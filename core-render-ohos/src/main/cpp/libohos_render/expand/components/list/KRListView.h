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

#ifndef CORE_RENDER_OHOS_KRLISTVIEW_H
#define CORE_RENDER_OHOS_KRLISTVIEW_H

#include "libohos_render/expand/components/view/SuperTouchHandler.h"
#include "libohos_render/export/IKRRenderViewExport.h"
#include <cstdint>
#include <memory>

class KRListAdapter;
class KRListView : public IKRRenderViewExport {
public:
    KRListView();
    virtual ~KRListView();

    ArkUI_NodeHandle CreateNode() override;
    bool ReuseEnable() override;
    void OnEvent(ArkUI_NodeEvent *event, const ArkUI_NodeEventType &event_type) override;
    bool SetProp(const std::string &prop_key, const KRAnyValue &prop_value,
                 const KRRenderCallback event_call_back = nullptr) override;
    bool ResetProp(const std::string &prop_key) override;
    void WillRemoveFromParentView() override;
    void DidMoveToParentView() override;
    void DidInit() override;
    ArkUI_GestureInterruptResult OnInterruptGestureEvent(const ArkUI_GestureInterruptInfo *info) override;
    void CallMethod(const std::string &method, const KRAnyValue &params, const KRRenderCallback &callback) override;
    bool CanPullRefresh() override;
    void updateContentOffsetFromParent(float offset) override;

private:
    ArkUI_NodeHandle ark_ui_node_handler_ = nullptr;
    std::shared_ptr<KRListAdapter> list_adapter_;
    KRRenderCallback on_scroll_call_back_;
    KRRenderCallback on_scroll_start_call_back_;
    KRRenderCallback on_scroll_end_call_back_;
    KRRenderCallback on_visible_changed_call_back_;
    int32_t first_visible_position_ = 0;
    int32_t last_visible_position_ = 0;
    int64_t last_scroll_time_ = 0;
    bool is_vertical_ = true;
    int32_t scroll_event_throttle_ = 60;
    std::weak_ptr<SuperTouchHandler> weak_super_touch_handler_;
    bool scroll_enable_ = true;
    float float_content_offset_ = 0;
    float content_offset_update_from_parent_ = 0;
    int32_t first_visible_index_ = 0;
    bool pending_scroll_callback_after_data_update_ = false;
    bool list_area_event_registered_ = false;
    bool first_size_set_ = false;
    float last_list_width_ = 0;
    float last_list_height_ = 0;

    bool SetIsVertical(bool isVertical);
    float GetFirstVisibleItemOffset();
    void OnScroll(ArkUI_NodeEvent *event);
    void OnScrollFrameBegin(ArkUI_NodeEvent *event);
    void OnScrollStart(ArkUI_NodeEvent *event);
    void OnScrollEnd(ArkUI_NodeEvent *event);
    void OnVisibleContentChanged(ArkUI_NodeEvent *event);
    void OnListAreaChange(ArkUI_NodeEvent *event);
    bool SetOnVisibleChanged(const KRRenderCallback &callback);
    bool SetOnScrollCallback(const KRRenderCallback &callback);
    bool SetOnScrollStartCallback(const KRRenderCallback &callback);
    bool SetOnScrollEndCallback(const KRRenderCallback &callback);
    bool SetScrollEventThrottle(int throttle);
    std::shared_ptr<KRRenderValue> GetCommonScrollParams();
    void updateInitialData(const std::string &params);
    void ScrollToIndex(const std::string &params);
    void updateData(const std::string &params);
    bool SetFreezingItems(const std::string &items);
    void CallOnScrollCallback();
    static void StaticOnListAreaChange(ArkUI_NodeEvent *event);
    void Dispose();

    KRRenderCallback ConvertToKCMPCallback(const KRRenderCallback &callback);
};

#endif  // CORE_RENDER_OHOS_KRLISTVIEW_H
