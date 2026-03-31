#include "libohos_render/expand/components/refresh/KRPullRefreshView.h"
#include "libohos_render/utils/KRJSONObject.h"
#include <cmath>

static constexpr const char *ON_LOADING_SHOW = "onLoadingShow";
static constexpr const char *METHOD_DISMISS_LOADING = "dismissLoading";
static constexpr const char *LAYOUT_INFO = "layoutInfo";
static constexpr const char *LOADING_HEIGHT = "loadingHeight";
static constexpr const char *LOADING_X = "loadingX";

static constexpr int REBOUND_ANIMATION_DURATION = 300;
static constexpr float ANGLE_45_TAN = 1.0f;
static constexpr float MIN_DETECT_OFFSET = 5.0f;

ArkUI_NodeHandle KRPullRefreshView::CreateNode() {
    ark_ui_node_handler_ = kuikly::util::GetNodeApi()->createNode(ARKUI_NODE_STACK);
    ArkUI_NumberValue value[] = {{.i32 = 1}};
    ArkUI_AttributeItem Item = {.value = value, .size = 1};
    kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_CLIP, &Item);
    return ark_ui_node_handler_;
}

bool KRPullRefreshView::ReuseEnable() {
    return false;
}

void KRPullRefreshView::DidInit() {
    RegisterEvent(NODE_TOUCH_EVENT);
    RegisterEvent(NODE_ON_TOUCH_INTERCEPT);
}

void KRPullRefreshView::DidInsertSubRenderView(const std::shared_ptr<IKRRenderViewExport> &sub_render_view, int index) {
    IKRRenderViewExport::DidInsertSubRenderView(sub_render_view, index);
    if (index == 0) {
        loading_view_export_ = sub_render_view;
    } else {
        content_view_export_ = sub_render_view;
    }
}

bool KRPullRefreshView::SetProp(const std::string &prop_key, const KRAnyValue &prop_value,
                                const KRRenderCallback event_callback) {
    if (prop_key == ON_LOADING_SHOW) {
        loading_show_callback_ = event_callback;
        return true;
    } else if (prop_key == LAYOUT_INFO) {
        SetLayoutInfo(prop_value->toString());
        return true;
    }
    return false;
}

void KRPullRefreshView::OnEvent(ArkUI_NodeEvent *event, const ArkUI_NodeEventType &event_type) {
    if (event_type == NODE_TOUCH_EVENT) {
        ProcessTouchEvent(event);
    } else if (event_type == NODE_ON_TOUCH_INTERCEPT) {
        if (is_refreshing_) return;
        auto input_event = kuikly::util::GetArkUIInputEvent(event);
        if (input_event && current_offset_y_ > 0) {
            OH_ArkUI_PointerEvent_SetInterceptHitTestMode(input_event, HTM_BLOCK);
        }
    }
}

void KRPullRefreshView::SetLayoutInfo(std::string info) {
    if (info.empty()) return;
    auto json = kuikly::util::JSONObject::Parse(info);
    if (!json) return;
    loading_x_ = json->GetNumber(LOADING_X);
    loading_height_ = json->GetNumber(LOADING_HEIGHT);
    if (loading_view_export_) {
        SetPosition(loading_view_export_->GetNode(), loading_x_, -loading_height_);
    }
}

void KRPullRefreshView::ProcessTouchEvent(ArkUI_NodeEvent *event) {
    auto input_event = kuikly::util::GetArkUIInputEvent(event);
    auto action = kuikly::util::GetArkUIInputEventAction(input_event);

    if (action == UI_TOUCH_EVENT_ACTION_DOWN) {
        OnTouchDownEvent(input_event);
    } else if (action == UI_TOUCH_EVENT_ACTION_MOVE) {
        OnTouchMoveEvent(input_event);
    } else if (action == UI_TOUCH_EVENT_ACTION_UP) {
        OnTouchUpEvent(input_event);
    } else if (action == UI_TOUCH_EVENT_ACTION_CANCEL) {
        OnTouchCancelEvent(input_event);
    }
}

void KRPullRefreshView::OnTouchDownEvent(ArkUI_UIInputEvent *input_event) {
    auto point = kuikly::util::GetArkUIInputEventPoint(input_event, 0);
    start_point_x_ = point.x;
    start_point_y_ = point.y;
    last_point_y_ = point.y;
    is_horizontal_slide_ = false;
    is_direction_locked_ = false;
}

void KRPullRefreshView::OnTouchMoveEvent(ArkUI_UIInputEvent *input_event) {
    if (is_refreshing_ || is_horizontal_slide_) return;
    UpdateLoading(input_event);
}

void KRPullRefreshView::OnTouchUpEvent(ArkUI_UIInputEvent *input_event) {
    if (is_refreshing_) return;
    EnableContentScroll(true);
    if (is_horizontal_slide_) {
        ResetGestureState();
        return;
    }
    FinishSpinner();
}

void KRPullRefreshView::OnTouchCancelEvent(ArkUI_UIInputEvent *input_event) {
    if (is_refreshing_) return;
    EnableContentScroll(true);
    if (is_horizontal_slide_) {
        ResetGestureState();
        return;
    }
    FinishSpinner();
}

void KRPullRefreshView::ResetGestureState() {
    if (!content_view_export_ || !loading_view_export_) return;
    current_offset_y_ = 0;
    SetPosition(loading_view_export_->GetNode(), loading_x_, -loading_height_);
    SetPosition(content_view_export_->GetNode(), 0, 0);
}

void KRPullRefreshView::UpdateLoading(ArkUI_UIInputEvent *input_event) {
    if (!input_event || !content_view_export_ || !loading_view_export_) return;
    if (current_offset_y_ <= 0 && !content_view_export_->CanPullRefresh()) {
        return;
    }

    auto pointer_count = kuikly::util::GetArkUIInputEventPointerCount(input_event);
    if (pointer_count != 1) return;

    auto point = kuikly::util::GetArkUIInputEventPoint(input_event, 0);
    float point_x = point.x;
    float point_y = point.y;

    if (point_y == last_point_y_) return;

    if (!is_direction_locked_) {
        float total_delta_x = fabs(point_x - start_point_x_);
        float total_delta_y = fabs(point_y - start_point_y_);
        if (total_delta_x > MIN_DETECT_OFFSET && total_delta_y > 0) {
            float slide_ratio = total_delta_x / total_delta_y;
            if (slide_ratio >= ANGLE_45_TAN) {
                is_horizontal_slide_ = true;
                is_direction_locked_ = true;
                last_point_y_ = point_y;
                ResetGestureState();
                return;
            } else {
                is_direction_locked_ = true;
                last_point_y_ = point_y;
                return;
            }
        } else if (total_delta_y > MIN_DETECT_OFFSET) {
            is_direction_locked_ = true;
            last_point_y_ = point_y;
            return;
        } else {
            last_point_y_ = point_y;
            return;
        }
    }

    float delta_y = point_y - last_point_y_;
    float new_offset = current_offset_y_ + delta_y;
    if (new_offset < 0) new_offset = 0;

    SetPosition(loading_view_export_->GetNode(), loading_x_, new_offset - loading_height_);
    SetPosition(content_view_export_->GetNode(), 0, new_offset);

    last_point_y_ = point_y;
    float prev_offset = current_offset_y_;
    current_offset_y_ = new_offset;
    bool should_block = current_offset_y_ > 0;
    bool was_block = prev_offset > 0;

    if (was_block != should_block) {
        EnableContentScroll(!should_block);
    }

    content_view_export_->updateContentOffsetFromParent(current_offset_y_);
}

void KRPullRefreshView::FinishSpinner() {
    if (current_offset_y_ <= 0) return;

    if (current_offset_y_ >= loading_height_) {
        is_refreshing_ = true;
        if (loading_show_callback_) {
            loading_show_callback_(nullptr);
        }
        if (current_offset_y_ > loading_height_) {
            ReboundToLoadingHeight();
        }
    } else {
        HideLoading();
    }
}

void KRPullRefreshView::ReboundToLoadingHeight() {
    if (!content_view_export_ || !loading_view_export_) return;
    auto root_view = GetRootView().lock();
    if (!root_view || !content_view_export_ || !loading_view_export_) {
        SetPosition(loading_view_export_->GetNode(), loading_x_, 0);
        SetPosition(content_view_export_->GetNode(), 0, loading_height_);
        current_offset_y_ = loading_height_;
        return;
    }

    auto animate_option = std::make_shared<KRAnimateOption>();
    animate_option->SetDuration(REBOUND_ANIMATION_DURATION);
    std::weak_ptr<KRPullRefreshView> weakSelf = std::dynamic_pointer_cast<KRPullRefreshView>(shared_from_this());

    content_inset_animate_ = std::make_shared<KRAnimation>(
            root_view->GetUIContextHandle(), animate_option, [weakSelf]() {
                if (auto strongSelf = weakSelf.lock()) {
                    strongSelf->SetPosition(strongSelf->loading_view_export_->GetNode(), strongSelf->loading_x_, 0);
                    strongSelf->SetPosition(strongSelf->content_view_export_->GetNode(), 0, strongSelf->loading_height_);
                }
            });

    content_inset_animate_->SetCompleteCallback(
            ArkUI_FinishCallbackType::ARKUI_FINISH_CALLBACK_LOGICALLY, [weakSelf]() {
                if (std::shared_ptr<KRPullRefreshView> strongSelf = weakSelf.lock()) {
                    strongSelf->SetPosition(strongSelf->loading_view_export_->GetNode(), strongSelf->loading_x_, 0);
                    strongSelf->SetPosition(strongSelf->content_view_export_->GetNode(), 0, strongSelf->loading_height_);
                    strongSelf->current_offset_y_ = strongSelf->loading_height_;
                    strongSelf->content_inset_animate_ = nullptr;
                    strongSelf->content_view_export_->updateContentOffsetFromParent(strongSelf->loading_height_);
                }
            });

    content_inset_animate_->Start();
}

void KRPullRefreshView::HideLoading() {
    if (!content_view_export_ || !loading_view_export_) return;
    auto root_view = GetRootView().lock();

    if (!root_view) {
        SetPosition(loading_view_export_->GetNode(), loading_x_, -loading_height_);
        SetPosition(content_view_export_->GetNode(), 0, 0);
        current_offset_y_ = 0;
        is_refreshing_ = false;
        content_view_export_->updateContentOffsetFromParent(0);
        return;
    }

    hide_generation_++;
    int generation = hide_generation_;
    auto animate_option = std::make_shared<KRAnimateOption>();
    animate_option->SetDuration(REBOUND_ANIMATION_DURATION);
    std::weak_ptr<KRPullRefreshView> weakSelf = std::dynamic_pointer_cast<KRPullRefreshView>(shared_from_this());

    hide_animate_ = std::make_shared<KRAnimation>(
            root_view->GetUIContextHandle(), animate_option, [weakSelf, generation]() {
                if (std::shared_ptr<KRPullRefreshView> strongSelf = weakSelf.lock()) {
                    if (strongSelf->hide_generation_ != generation) return;
                    strongSelf->SetPosition(strongSelf->loading_view_export_->GetNode(), strongSelf->loading_x_,
                                            -strongSelf->loading_height_);
                    strongSelf->SetPosition(strongSelf->content_view_export_->GetNode(), 0, 0);
                }
            });

    hide_animate_->SetCompleteCallback(
            ArkUI_FinishCallbackType::ARKUI_FINISH_CALLBACK_LOGICALLY, [weakSelf, generation]() {
                if (std::shared_ptr<KRPullRefreshView> strongSelf = weakSelf.lock()) {
                    if (strongSelf->hide_generation_ != generation) return;
                    strongSelf->hide_animate_ = nullptr;
                    strongSelf->current_offset_y_ = 0;
                    strongSelf->is_refreshing_ = false;
                    strongSelf->content_view_export_->updateContentOffsetFromParent(0);
                }
            });

    hide_animate_->Start();
}

void KRPullRefreshView::SetPosition(ArkUI_NodeHandle node, float x, float y) {
    ArkUI_NumberValue value[] = {{.f32 = x}, {.f32 = y}};
    ArkUI_AttributeItem Item = {.value = value, .size = 2};
    kuikly::util::GetNodeApi()->setAttribute(node, NODE_POSITION, &Item);
}

bool KRPullRefreshView::ResetProp(const std::string &prop_key) {
    return false;
}

void KRPullRefreshView::EnableContentScroll(bool enable) {
    if (!content_view_export_) return;
    ArkUI_NumberValue value[] = {{.i32 = enable ? 1 : 0}};
    ArkUI_AttributeItem item = {.value = value, .size = 1};
    kuikly::util::GetNodeApi()->setAttribute(
        content_view_export_->GetNode(), NODE_SCROLL_ENABLE_SCROLL_INTERACTION, &item);
}

void KRPullRefreshView::CallMethod(const std::string &method, const KRAnyValue &params, const KRRenderCallback &callback) {
    if (method == METHOD_DISMISS_LOADING) {
        if (is_refreshing_ && !hide_animate_) {
            HideLoading();
        }
    } else {
        IKRRenderViewExport::CallMethod(method, params, callback);
    }
}
