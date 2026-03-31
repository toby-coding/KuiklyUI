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

#include "libohos_render/expand/components/list/KRSyncSwiper.h"
#include "libohos_render/expand/components/list/KRListAdapter.h"
#include "libohos_render/expand/components/view/KRView.h"
#include "libohos_render/utils/KRJSONObject.h"
#include "libohos_render/utils/KRViewUtil.h"
#include <algorithm>

extern void *OH_ArkUI_GestureInterrupter_GetUserData(ArkUI_GestureInterruptInfo *event) __attribute__((weak));

static constexpr const char *ON_SCROLL = "onScroll";
static constexpr const char *ON_SCROLL_START = "onScrollStart";
static constexpr const char *ON_SCROLL_END = "onScrollEnd";
static constexpr const char *CONTENT_OFFSET = "contentOffset";
static constexpr const char *IS_VERTICAL = "isVertical";
static constexpr const char *SCROLL_EVENT_THROTTLE = "scrollEventThrottle";
static constexpr const char *FIRST_VISIBLE_INDEX = "firstVisibleIndex";
static constexpr const char *LAST_VISIBLE_INDEX = "lastVisibleIndex";
static constexpr const char *ON_VISIBLE_CHANGE = "onVisibleChange";
static constexpr const char *INITIAL_RENDER_COUNT = "initialRenderCount";
static constexpr const char *ON_BIND = "onBind";
static constexpr const char *UPDATE_DATA = "updateData";
static constexpr const char *UPDATE_INITIAL_DATA = "updateInitialData";
static constexpr const char *REMOVE = "remove";
static constexpr const char *INSERT = "insert";
static constexpr const char *UPDATE = "update";
static constexpr const char *ITEM_CACHE_SIZE = "itemCacheSize";
static constexpr const char *SCROLL_ENABLED = "scrollEnabled";
static constexpr const char *SCROLL_TO_INDEX = "scrollToIndex";
static constexpr const char *SCROLL_BY = "scrollBy";
static constexpr const char *ANIMATION = "animation";
static constexpr const char *FLING_ENABLE = "flingEnable";
static constexpr const char *INITIAL_INDEX = "initialIndex";
static constexpr const char *INDEX = "index";

KRSyncSwiper::KRSyncSwiper() {
    is_vertical_ = false;
    scroll_event_throttle_ = 60;
}

KRSyncSwiper::~KRSyncSwiper() {}

ArkUI_NodeHandle KRSyncSwiper::CreateNode() {
    ark_ui_node_handler_ = kuikly::util::GetNodeApi()->createNode(ARKUI_NODE_SWIPER);
    list_adapter_ = std::make_shared<KRListAdapter>();

    ArkUI_AttributeItem item{nullptr, 0, nullptr, list_adapter_->GetHandle()};
    kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_SWIPER_NODE_ADAPTER, &item);

    ArkUI_NumberValue loop_value[] = {{.i32 = 0}};
    ArkUI_AttributeItem loop_item = {.value = loop_value, .size = 1};
    kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_SWIPER_LOOP, &loop_item);

    ArkUI_NumberValue indicator_value[] = {{.i32 = 0}};
    ArkUI_AttributeItem indicator_item = {.value = indicator_value, .size = 1};
    kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_SWIPER_SHOW_INDICATOR, &indicator_item);

    return ark_ui_node_handler_;
}

bool KRSyncSwiper::ReuseEnable() {
    return false;
}

void KRSyncSwiper::DidInit() {
    RegisterEvent(NODE_SWIPER_EVENT_ON_ANIMATION_START);
    RegisterEvent(NODE_SWIPER_EVENT_ON_ANIMATION_END);
}

KRRenderCallback KRSyncSwiper::ConvertToKCMPCallback(const KRRenderCallback &callback) {
    return callback;
}

void KRSyncSwiper::OnEvent(ArkUI_NodeEvent *event, const ArkUI_NodeEventType &event_type) {
    if (event_type == NODE_SWIPER_EVENT_ON_ANIMATION_START) {
        OnScrollStart(event);
    } else if (event_type == NODE_SWIPER_EVENT_ON_ANIMATION_END) {
        OnScrollEnd(event);
    }
}

void KRSyncSwiper::OnScrollStart(ArkUI_NodeEvent *event) {
    if (auto handler = weak_super_touch_handler_.lock()) {
        handler->SetNativeTouchConsumer(shared_from_this());
    }

    if (on_scroll_start_call_back_) {
        on_scroll_start_call_back_(nullptr);
    }
}

void KRSyncSwiper::OnScrollEnd(ArkUI_NodeEvent *event) {
    ArkUI_NodeComponentEvent *compEvent = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);

    if (on_scroll_call_back_) {
        on_scroll_call_back_(GetCommonScrollParams(compEvent->data[0].i32));
    }

    OnVisibleContentChanged(compEvent->data[0].i32);

    if (on_scroll_end_call_back_) {
        on_scroll_end_call_back_(nullptr);
    }

    if (auto handler = weak_super_touch_handler_.lock()) {
        handler->ClearNativeTouchConsumer(shared_from_this());
    }
}

void KRSyncSwiper::OnVisibleContentChanged(int32_t index) {
    if (on_visible_changed_call_back_) {
        std::unordered_map<std::string, KRAnyValue> map;
        map[FIRST_VISIBLE_INDEX] = NewKRRenderValue(index);
        map[LAST_VISIBLE_INDEX] = NewKRRenderValue(index);
        on_visible_changed_call_back_(NewKRRenderValue(map));
    }
}

std::shared_ptr<KRRenderValue> KRSyncSwiper::GetCommonScrollParams(int32_t index) {
    std::unordered_map<std::string, KRAnyValue> map;
    float offset = 0.0f;

    if (is_vertical_) {
        auto item = kuikly::util::GetNodeApi()->getAttribute(ark_ui_node_handler_, NODE_HEIGHT);
        offset = item ? item->value[0].f32 : 0;
    } else {
        auto item = kuikly::util::GetNodeApi()->getAttribute(ark_ui_node_handler_, NODE_WIDTH);
        offset = item ? item->value[0].f32 : 0;
    }

    map[CONTENT_OFFSET] = NewKRRenderValue(index * offset);
    return std::make_shared<KRRenderValue>(map);
}

bool KRSyncSwiper::SetProp(const std::string &prop_key, const KRAnyValue &prop_value,
                           const KRRenderCallback event_callback) {
    KRRenderCallback render_callback = ConvertToKCMPCallback(event_callback);

    if (prop_key == IS_VERTICAL) {
        is_vertical_ = prop_value->toBool();
        int32_t direction = is_vertical_ ? 1 : 0;
        ArkUI_NumberValue value[] = {{.i32 = direction}};
        ArkUI_AttributeItem Item = {.value = value, .size = 1};
        kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_SWIPER_VERTICAL, &Item);
        return true;
    } else if (prop_key == ITEM_CACHE_SIZE) {
        ArkUI_NumberValue value[] = {{.i32 = prop_value->toInt()}};
        ArkUI_AttributeItem Item = {.value = value, .size = 1};
        kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_SWIPER_CACHED_COUNT, &Item);
        return true;
    } else if (prop_key == SCROLL_ENABLED) {
        ArkUI_NumberValue value[] = {{.i32 = prop_value->toBool() ? 1 : 0}};
        ArkUI_AttributeItem Item = {.value = value, .size = 1};
        kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_SCROLL_ENABLE_SCROLL_INTERACTION, &Item);
        return true;
    } else if (prop_key == INITIAL_INDEX) {
        initial_page_ = prop_value->toInt();
        ArkUI_NumberValue value[] = {{.i32 = initial_page_}, {.i32 = 0}};
        ArkUI_AttributeItem Item = {.value = value, .size = 2};
        kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_SWIPER_INDEX, &Item);
        return true;
    } else if (prop_key == ON_SCROLL) {
        return SetOnScrollCallback(render_callback);
    } else if (prop_key == ON_SCROLL_START) {
        return SetOnScrollStartCallback(render_callback);
    } else if (prop_key == ON_SCROLL_END) {
        return SetOnScrollEndCallback(render_callback);
    } else if (prop_key == ON_VISIBLE_CHANGE) {
        return SetOnVisibleChanged(render_callback);
    }

    return false;
}

void KRSyncSwiper::CallMethod(const std::string &method, const KRAnyValue &params,
                              const KRRenderCallback &callback) {
    KRRenderCallback render_callback = ConvertToKCMPCallback(callback);
    if (method == ON_BIND) {
        if (list_adapter_) {
            list_adapter_->setOnBindCallback(render_callback);
        }
    }
    if (method == UPDATE_DATA)
        UpdateData(params->toString());
    if (method == UPDATE_INITIAL_DATA)
        UpdateInitialData(params->toString());
    if (method == SCROLL_TO_INDEX)
        ScrollToIndex(params->toString());
}

void KRSyncSwiper::ScrollToIndex(const std::string &params) {
    auto json = kuikly::util::JSONObject::Parse(params);
    int index = json->GetNumber(INDEX);
    int animation = json->GetNumber(ANIMATION) == 1;
    ArkUI_NumberValue value[] = {{.i32 = index}, {.i32 = animation}};
    ArkUI_AttributeItem Item = {.value = value, .size = 2};
    kuikly::util::GetNodeApi()->setAttribute(ark_ui_node_handler_, NODE_SWIPER_INDEX, &Item);

    if (on_scroll_call_back_ && initial_page_ != 0) {
        on_scroll_call_back_(GetCommonScrollParams(index));
    }
}

void KRSyncSwiper::UpdateData(const std::string &params) {
    KR_LOG_INFO << "KRSyncSwiper UpdateData params:" << params;
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
    }
}

void KRSyncSwiper::UpdateInitialData(const std::string &params) {
    KR_LOG_INFO << "KRSyncSwiper UpdateInitialData params:" << params;
    if (params.empty() || !list_adapter_) return;

    auto json = kuikly::util::JSONObject::Parse(params);
    int initialRenderCount = json->GetNumber(INITIAL_RENDER_COUNT);
    list_adapter_->SetInitialCount(initialRenderCount);

    if (on_scroll_call_back_ && initial_page_ != 0) {
        on_scroll_call_back_(GetCommonScrollParams(initial_page_));
    }
}

bool KRSyncSwiper::SetOnScrollCallback(const KRRenderCallback &callback) {
    on_scroll_call_back_ = callback;
    IKRRenderViewExport::RegisterEvent(NODE_SCROLL_EVENT_ON_SCROLL);
    return true;
}

bool KRSyncSwiper::SetOnScrollStartCallback(const KRRenderCallback &callback) {
    on_scroll_start_call_back_ = callback;
    return true;
}

bool KRSyncSwiper::SetOnVisibleChanged(const KRRenderCallback &callback) {
    on_visible_changed_call_back_ = callback;
    IKRRenderViewExport::RegisterEvent(NODE_LIST_ON_SCROLL_VISIBLE_CONTENT_CHANGE);
    return true;
}

bool KRSyncSwiper::SetOnScrollEndCallback(const KRRenderCallback &callback) {
    on_scroll_end_call_back_ = callback;
    return true;
}

bool KRSyncSwiper::ResetProp(const std::string &prop_key) {
    return true;
}

void KRSyncSwiper::DidMoveToParentView() {
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

void KRSyncSwiper::WillRemoveFromParentView() {
    IKRRenderViewExport::WillRemoveFromParentView();
    weak_super_touch_handler_.reset();
}

ArkUI_GestureInterruptResult KRSyncSwiper::OnInterruptGestureEvent(const ArkUI_GestureInterruptInfo *info) {
    if (auto handler = weak_super_touch_handler_.lock()) {
        auto recognizer = OH_ArkUI_GestureInterruptInfo_GetRecognizer(info);
        handler->CollectGestureRecognizer(recognizer);
        if (handler->IsPreventTouch()) {
            return GESTURE_INTERRUPT_RESULT_REJECT;
        }
    }
    return IKRRenderViewExport::OnInterruptGestureEvent(info);
}
