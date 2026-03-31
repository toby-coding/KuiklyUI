/*
 * Tencent is pleased to support the open source community by making KuiklyUI
 * available.
 * Copyright (C) 2025 Tencent. All rights reserved.
 * Licensed under the License of KuiklyUI;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * https://github.com/Tencent-TDS/KuiklyUI/blob/main/LICENSE
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "libohos_render/expand/components/list/KRListView.h"
#include "libohos_render/expand/components/list/KRListAdapter.h"
#include "libohos_render/expand/components/view/KRView.h"
#include "libohos_render/utils/KRJSONObject.h"
#include "libohos_render/utils/KRViewUtil.h"
#include <algorithm>
#include <cmath>
#include <chrono>

extern void *OH_ArkUI_GestureInterrupter_GetUserData(ArkUI_GestureInterruptInfo *event) __attribute__((weak));

static constexpr const char *IS_VERTICAL = "isVertical";
static constexpr const char *ITEM_CACHE_SIZE = "itemCacheSize";
static constexpr const char *SCROLL_ENABLED = "scrollEnabled";
static constexpr const char *FLING_ENABLE = "flingEnable";
static constexpr const char *DURATION = "duration";
static constexpr const char *ON_SCROLL = "onScroll";
static constexpr const char *ON_SCROLL_START = "onScrollStart";
static constexpr const char *ON_SCROLL_END = "onScrollEnd";
static constexpr const char *CONTENT_OFFSET = "contentOffset";
static constexpr const char *SCROLL_EVENT_THROTTLE = "scrollEventThrottle";
static constexpr const char *FIRST_VISIBLE_INDEX = "firstVisibleIndex";
static constexpr const char *FIRST_VISIBLE_ITEM_IN_VIEW_PORT_OFFSET = "firstVisibleItemInViewPortOffset";
static constexpr const char *EXTRA_CONTENT_OFFSET = "extraContentOffset";
static constexpr const char *LAST_VISIBLE_INDEX = "lastVisibleIndex";
static constexpr const char *ON_VISIBLE_CHANGE = "onVisibleChange";
static constexpr const char *UPDATE_INITIAL_DATA = "updateInitialData";
static constexpr const char *INITIAL_RENDER_COUNT = "initialRenderCount";
static constexpr const char *ON_BIND = "onBind";
static constexpr const char *UPDATE_DATA = "updateData";
static constexpr const char *SCROLL_TO_INDEX = "scrollToIndex";
static constexpr const char *SCROLL_BY = "scrollBy";
static constexpr const char *REMOVE = "remove";
static constexpr const char *INSERT = "insert";
static constexpr const char *UPDATE = "update";
static constexpr const char *ANIMATION = "animation";
static constexpr const char *INITIAL_INDEX = "initialIndex";
static constexpr const char *FREEZING_ITEMS = "freezingItems";
static constexpr const char *BOUNCES_ENABLED = "bouncesEnable";
static constexpr const char *INDEX = "index";

KRListView::KRListView() {
    is_vertical_ = true;
    scroll_event_throttle_ = 60;
}

KRListView::~KRListView() {
    if (KRISMainThread()) {
        Dispose();
    }
}

void KRListView::Dispose() {
    if (list_area_event_registered_ && ark_ui_node_handler_) {
        if (kuikly::util::ArkUINativeNodeAPI::GetInstance()->IsNodeAlive(ark_ui_node_handler_)) {
            kuikly::util::GetNodeApi()->unregisterNodeEvent(ark_ui_node_handler_, NODE_EVENT_ON_AREA_CHANGE);
            kuikly::util::GetNodeApi()->removeNodeEventReceiver(ark_ui_node_handler_, StaticOnListAreaChange);
        }
        list_area_event_registered_ = false;
    }
}

ArkUI_NodeHandle KRListView::CreateNode() {
    ark_ui_node_handler_ = kuikly::util::GetNodeApi()->createNode(ARKUI_NODE_LIST);
    list_adapter_ = std::make_shared<KRListAdapter>();

    ArkUI_AttributeItem item{nullptr, 0, nullptr, list_adapter_->GetHandle()};
    kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_LIST_ITEM_GROUP_NODE_ADAPTER, &item);

    kuikly::util::GetNodeApi()->addNodeEventReceiver(ark_ui_node_handler_, StaticOnListAreaChange);
    kuikly::util::GetNodeApi()->registerNodeEvent(ark_ui_node_handler_, NODE_EVENT_ON_AREA_CHANGE, 0, this);
    list_area_event_registered_ = true;

    ArkUI_NumberValue scroll_value[] = {{.i32 = ARKUI_SCROLL_BAR_DISPLAY_MODE_OFF}};
    ArkUI_AttributeItem show_scroll_indicator_item = {.value = scroll_value, .size = 1};
    kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_SCROLL_BAR_DISPLAY_MODE,
                                              &show_scroll_indicator_item);

    return ark_ui_node_handler_;
}

bool KRListView::ReuseEnable() {
    return false;
}

void KRListView::DidInit() {
    RegisterEvent(NODE_SCROLL_EVENT_ON_SCROLL_FRAME_BEGIN);
    RegisterEvent(NODE_SCROLL_EVENT_ON_SCROLL_START);
    RegisterEvent(NODE_SCROLL_EVENT_ON_WILL_SCROLL);
    RegisterEvent(NODE_SCROLL_EVENT_ON_SCROLL_STOP);
}

KRRenderCallback KRListView::ConvertToKCMPCallback(const KRRenderCallback &callback) {
    return callback;
}

void KRListView::OnEvent(ArkUI_NodeEvent *event, const ArkUI_NodeEventType &event_type) {
    if (event_type == NODE_SCROLL_EVENT_ON_SCROLL) {
        OnScroll(event);
    } else if (event_type == NODE_SCROLL_EVENT_ON_SCROLL_FRAME_BEGIN) {
        OnScrollFrameBegin(event);
    } else if (event_type == NODE_SCROLL_EVENT_ON_SCROLL_START) {
        OnScrollStart(event);
    } else if (event_type == NODE_SCROLL_EVENT_ON_SCROLL_STOP) {
        OnScrollEnd(event);
    } else if (event_type == NODE_LIST_ON_SCROLL_VISIBLE_CONTENT_CHANGE) {
        OnVisibleContentChanged(event);
    } else if (event_type == NODE_EVENT_ON_AREA_CHANGE) {
        OnListAreaChange(event);
    }
}

void KRListView::StaticOnListAreaChange(ArkUI_NodeEvent *event) {
    if (!event) return;
    auto list = reinterpret_cast<KRListView *>(OH_ArkUI_NodeEvent_GetUserData(event));
    if (list) {
        list->OnListAreaChange(event);
    }
}

void KRListView::OnListAreaChange(ArkUI_NodeEvent *event) {
    if (!event || !ark_ui_node_handler_) return;

    auto item_width = kuikly::util::GetNodeApi()->getAttribute(ark_ui_node_handler_, NODE_WIDTH);
    auto item_height = kuikly::util::GetNodeApi()->getAttribute(ark_ui_node_handler_, NODE_HEIGHT);
    float current_width = item_width ? item_width->value[0].f32 : 0;
    float current_height = item_height ? item_height->value[0].f32 : 0;

    if (!first_size_set_) {
        first_size_set_ = true;
        last_list_width_ = current_width;
        last_list_height_ = current_height;
    } else {
        bool size_changed = (std::abs(current_width - last_list_width_) > 1.0f ||
                            std::abs(current_height - last_list_height_) > 1.0f);
        if (size_changed) {
            last_list_width_ = current_width;
            last_list_height_ = current_height;
            if (list_adapter_) {
                list_adapter_->ForceRelayout();
            }
        }
    }
}

void KRListView::OnScroll(ArkUI_NodeEvent *event) {
    auto point = kuikly::util::GetArkUIScrollContentOffset(GetNode());

    if (is_vertical_) {
        float_content_offset_ = point.y;
    } else {
        float_content_offset_ = point.x;
    }

    if (on_scroll_call_back_) {
        auto current_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();

        if (current_time - last_scroll_time_ < scroll_event_throttle_) {
            return;
        }

        last_scroll_time_ = current_time;
        CallOnScrollCallback();
    }
}

void KRListView::CallOnScrollCallback() {
    if (on_scroll_call_back_) {
        std::unordered_map<std::string, KRAnyValue> map;
        map[CONTENT_OFFSET] = NewKRRenderValue(float_content_offset_);
        map[FIRST_VISIBLE_INDEX] = NewKRRenderValue(first_visible_index_);
        map[FIRST_VISIBLE_ITEM_IN_VIEW_PORT_OFFSET] = NewKRRenderValue(GetFirstVisibleItemOffset());
        map[EXTRA_CONTENT_OFFSET] = NewKRRenderValue(content_offset_update_from_parent_);
        on_scroll_call_back_(NewKRRenderValue(map));
    }
}

void KRListView::OnScrollFrameBegin(ArkUI_NodeEvent *event) {
    if (auto handler = weak_super_touch_handler_.lock()) {
        handler->SetNativeTouchConsumer(shared_from_this());
    }
}

void KRListView::OnScrollStart(ArkUI_NodeEvent *event) {
    if (on_scroll_start_call_back_) {
        on_scroll_start_call_back_(nullptr);
    }
}

void KRListView::OnScrollEnd(ArkUI_NodeEvent *event) {
    if (on_scroll_call_back_) {
        auto point = kuikly::util::GetArkUIScrollContentOffset(GetNode());

        if (is_vertical_) {
            float_content_offset_ = point.y;
        } else {
            float_content_offset_ = point.x;
        }

        CallOnScrollCallback();
    }

    if (on_scroll_end_call_back_) {
        on_scroll_end_call_back_(nullptr);
    }

    if (auto handler = weak_super_touch_handler_.lock()) {
        handler->ClearNativeTouchConsumer(shared_from_this());
    }
}

void KRListView::OnVisibleContentChanged(ArkUI_NodeEvent *event) {
    if (on_visible_changed_call_back_) {
        ArkUI_NodeComponentEvent *compEvent = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);

        if (!compEvent) {
            KR_LOG_ERROR << "Invalid NODE_LIST_ON_SCROLL_VISIBLE_CONTENT_CHANGE event";
            return;
        }

        int32_t firstChildIndex = compEvent->data[0].i32;
        int32_t lastChildIndex = compEvent->data[3].i32;

        first_visible_index_ = firstChildIndex;

        std::unordered_map<std::string, KRAnyValue> map;
        map[FIRST_VISIBLE_INDEX] = NewKRRenderValue(firstChildIndex);
        map[LAST_VISIBLE_INDEX] = NewKRRenderValue(lastChildIndex);
        on_visible_changed_call_back_(NewKRRenderValue(map));
    }

    if (pending_scroll_callback_after_data_update_) {
        pending_scroll_callback_after_data_update_ = false;
        CallOnScrollCallback();
    }
}

std::shared_ptr<KRRenderValue> KRListView::GetCommonScrollParams() {
    std::unordered_map<std::string, KRAnyValue> map;
    auto point = kuikly::util::GetArkUIScrollContentOffset(GetNode());

    if (is_vertical_) {
        map[CONTENT_OFFSET] = NewKRRenderValue(point.y);
    } else {
        map[CONTENT_OFFSET] = NewKRRenderValue(point.x);
    }

    return std::make_shared<KRRenderValue>(map);
}

bool KRListView::SetProp(const std::string &prop_key, const KRAnyValue &prop_value,
                          const KRRenderCallback event_callback) {
    KRRenderCallback render_callback = ConvertToKCMPCallback(event_callback);

    if (prop_key == ON_SCROLL)
        return SetOnScrollCallback(render_callback);
    if (prop_key == ON_SCROLL_START)
        return SetOnScrollStartCallback(render_callback);
    if (prop_key == ON_SCROLL_END)
        return SetOnScrollEndCallback(render_callback);
    if (prop_key == SCROLL_EVENT_THROTTLE)
        return SetScrollEventThrottle(prop_value->toInt());
    if (prop_key == ON_VISIBLE_CHANGE)
        return SetOnVisibleChanged(render_callback);
    if (prop_key == IS_VERTICAL)
        return SetIsVertical(prop_value->toBool());
    if (prop_key == ITEM_CACHE_SIZE) {
        if (list_adapter_) {
            ArkUI_NumberValue value[] = {{.i32 = prop_value->toInt()}};
            ArkUI_AttributeItem Item = {.value = value, .size = 1};
            kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_LIST_CACHED_COUNT, &Item);
        }
        return true;
    }
    if (prop_key == SCROLL_ENABLED) {
        scroll_enable_ = prop_value->toBool();
        ArkUI_NumberValue value[] = {{.i32 = scroll_enable_ ? 1 : 0}};
        ArkUI_AttributeItem Item = {.value = value, .size = 1};
        kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_SCROLL_ENABLE_SCROLL_INTERACTION, &Item);
        return true;
    }
    if (prop_key == BOUNCES_ENABLED) {
        kuikly::util::SetArkUIBouncesEnabled(ark_ui_node_handler_, prop_value->toBool());
        return true;
    }
    if (prop_key == FLING_ENABLE) {
        return true;
    }
    if (prop_key == INITIAL_INDEX) {
        ArkUI_NumberValue value[] = {{.i32 = prop_value->toInt()}};
        ArkUI_AttributeItem Item = {.value = value, .size = 1};
        kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_LIST_INITIAL_INDEX, &Item);
        return true;
    }
    if (prop_key == FREEZING_ITEMS) {
        return SetFreezingItems(prop_value->toString());
    }

    return false;
}

bool KRListView::SetFreezingItems(const std::string &items) {
    auto item_index_splits = kuikly::util::SplitString(items, ' ');
    if (item_index_splits.empty()) {
        return true;
    }
    std::vector<int> item_indices;
    item_indices.reserve(item_index_splits.size());

    for (size_t i = 0; i < item_index_splits.size(); ++i) {
        int index = item_index_splits[i]->toInt();
        item_indices.push_back(index);
    }

    if (list_adapter_) {
        list_adapter_->SetFreezingItems(item_indices);
    }
    return true;
}

bool KRListView::SetIsVertical(bool isVertical) {
    is_vertical_ = isVertical;
    int32_t direction = isVertical ? ARKUI_AXIS_VERTICAL : ARKUI_AXIS_HORIZONTAL;
    ArkUI_NumberValue value[] = {{.i32 = direction}};
    ArkUI_AttributeItem Item = {.value = value, .size = 1};
    kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_LIST_DIRECTION, &Item);
    return true;
}

void KRListView::CallMethod(const std::string &method, const KRAnyValue &params,
                            const KRRenderCallback &callback) {
    if (method == UPDATE_INITIAL_DATA)
        updateInitialData(params->toString());
    if (method == ON_BIND) {
        if (list_adapter_) {
            list_adapter_->setOnBindCallback(ConvertToKCMPCallback(callback));
        }
    }
    if (method == UPDATE_DATA)
        updateData(params->toString());
    if (method == SCROLL_TO_INDEX)
        ScrollToIndex(params->toString());
    if (method == SCROLL_BY) {
        ArkUI_NumberValue value[] = {{.f32 = 0}, {.f32 = params->toFloat()}};
        ArkUI_AttributeItem Item = {.value = value, .size = 2};
        kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_SCROLL_BY, &Item);
    }
}

void KRListView::updateData(const std::string &params) {
    KR_LOG_INFO << "KRListView updateData params:" << params;
    if (params.empty() || !list_adapter_) return;

    auto json = kuikly::util::JSONObject::Parse(params);
    if (!json) return;

    bool should_update_index = false;
    std::string removeString = json->GetString(REMOVE);
    auto removeObject = kuikly::util::JSONObject::Parse(removeString);

    if (removeObject) {
        should_update_index = true;
        auto removeArray = removeObject->GetNumberArray("");
        std::vector<int> removeList(removeArray.begin(), removeArray.end());

        if (!removeList.empty()) {
            std::sort(removeList.rbegin(), removeList.rend());
            for (int index : removeList) {
                list_adapter_->RemoveItem(index);
            }
        }
    }

    std::string insertString = json->GetString(INSERT);
    auto insertObject = kuikly::util::JSONObject::Parse(insertString);

    if (insertObject) {
        should_update_index = true;
        auto insertArray = insertObject->GetNumberArray("");
        std::vector<int> insertList(insertArray.begin(), insertArray.end());

        if (!insertList.empty()) {
            std::sort(insertList.begin(), insertList.end());
            list_adapter_->InsertItem(insertList[0], insertList.size());
        }
    }

    std::string updateString = json->GetString(UPDATE);
    auto updateObject = kuikly::util::JSONObject::Parse(updateString);

    if (updateObject) {
        auto updateArray = updateObject->GetNumberArray("");
        std::vector<int> updateList(updateArray.begin(), updateArray.end());

        if (!updateList.empty()) {
            std::sort(updateList.begin(), updateList.end());
            for (int index : updateList) {
                list_adapter_->ReloadItem(index);
            }
        }
    }

    if (should_update_index) {
        list_adapter_->BatchUpdateComplete();
        pending_scroll_callback_after_data_update_ = true;
    }
}

bool KRListView::CanPullRefresh() {
    return float_content_offset_ == 0;
}

float KRListView::GetFirstVisibleItemOffset() {
    float offset = 0;
    if (list_adapter_) {
        auto first_item_handle = list_adapter_->GetItemNodeHandle(first_visible_index_);
        if (first_item_handle) {
            auto pos = kuikly::util::GetNodePositionInWindow(first_item_handle);
            auto list_pos = kuikly::util::GetNodePositionInWindow(GetNode());
            if (is_vertical_) {
                offset += (list_pos.y - pos.y);
            } else {
                offset += (list_pos.x - pos.x);
            }
        }
    }
    return offset;
}

void KRListView::updateContentOffsetFromParent(float offset) {
    content_offset_update_from_parent_ = offset;
    CallOnScrollCallback();
}

void KRListView::ScrollToIndex(const std::string &params) {
    if (params.empty()) return;

    auto json = kuikly::util::JSONObject::Parse(params);
    int index = json->GetNumber(INDEX);
    int animation = json->GetNumber(ANIMATION);
    ArkUI_NumberValue value[] = {{.i32 = index}, {.i32 = animation}};
    ArkUI_AttributeItem Item = {.value = value, .size = 2};
    kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_LIST_SCROLL_TO_INDEX, &Item);
}

void KRListView::updateInitialData(const std::string &params) {
    KR_LOG_INFO << "KRListView updateInitialData params:" << params;
    if (params.empty() || !list_adapter_) return;

    auto json = kuikly::util::JSONObject::Parse(params);
    int initialRenderCount = json->GetNumber(INITIAL_RENDER_COUNT);

    list_adapter_->SetInitialCount(initialRenderCount);
}

bool KRListView::SetScrollEventThrottle(int throttle) {
    scroll_event_throttle_ = 1000 / throttle;
    return true;
}

bool KRListView::SetOnScrollCallback(const KRRenderCallback &callback) {
    on_scroll_call_back_ = callback;
    IKRRenderViewExport::RegisterEvent(NODE_SCROLL_EVENT_ON_SCROLL);
    return true;
}

bool KRListView::SetOnScrollStartCallback(const KRRenderCallback &callback) {
    on_scroll_start_call_back_ = callback;
    return true;
}

bool KRListView::SetOnVisibleChanged(const KRRenderCallback &callback) {
    on_visible_changed_call_back_ = callback;
    IKRRenderViewExport::RegisterEvent(NODE_LIST_ON_SCROLL_VISIBLE_CONTENT_CHANGE);
    return true;
}

bool KRListView::SetOnScrollEndCallback(const KRRenderCallback &callback) {
    on_scroll_end_call_back_ = callback;
    return true;
}

bool KRListView::ResetProp(const std::string &prop_key) {
    return true;
}

void KRListView::DidMoveToParentView() {
    auto parent_view = GetParentView();
    while (parent_view != nullptr) {
        std::shared_ptr<SuperTouchHandler> handler = nullptr;

        if (auto view = std::dynamic_pointer_cast<KRView>(parent_view)) {
            handler = view->GetSuperTouchHandler();
        }

        if (handler) {
            weak_super_touch_handler_ = handler;
            if (!OH_ArkUI_GestureInterrupter_GetUserData) {
                SetViewTag(GetViewTag());
            }
            RegisterGestureInterrupter();
            break;
        }

        parent_view = parent_view->GetParentView();
    }
}

void KRListView::WillRemoveFromParentView() {
    IKRRenderViewExport::WillRemoveFromParentView();
    weak_super_touch_handler_.reset();
}

ArkUI_GestureInterruptResult KRListView::OnInterruptGestureEvent(const ArkUI_GestureInterruptInfo *info) {
    if (auto handler = weak_super_touch_handler_.lock()) {
        auto recognizer = OH_ArkUI_GestureInterruptInfo_GetRecognizer(info);
        handler->CollectGestureRecognizer(recognizer);
        if (handler->IsPreventTouch()) {
            return GESTURE_INTERRUPT_RESULT_REJECT;
        }
    }
    return IKRRenderViewExport::OnInterruptGestureEvent(info);
}
