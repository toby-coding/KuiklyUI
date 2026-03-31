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

#ifndef CORE_RENDER_OHOS_KRSYNCSWIPER_H
#define CORE_RENDER_OHOS_KRSYNCSWIPER_H

#include "libohos_render/expand/components/view/SuperTouchHandler.h"
#include "libohos_render/export/IKRRenderViewExport.h"
#include <cstdint>
#include <memory>

class KRListAdapter;
class KRSyncSwiper : public IKRRenderViewExport {
public:
    KRSyncSwiper();
    virtual ~KRSyncSwiper();

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

private:
    ArkUI_NodeHandle ark_ui_node_handler_ = nullptr;
    std::shared_ptr<KRListAdapter> list_adapter_;
    KRRenderCallback on_scroll_call_back_;
    KRRenderCallback on_scroll_start_call_back_;
    KRRenderCallback on_scroll_end_call_back_;
    KRRenderCallback on_visible_changed_call_back_;
    bool is_vertical_ = false;
    int initial_page_ = 0;
    int32_t scroll_event_throttle_ = 60;
    std::weak_ptr<SuperTouchHandler> weak_super_touch_handler_;

    void OnScrollStart(ArkUI_NodeEvent *event);
    void OnScrollEnd(ArkUI_NodeEvent *event);
    void OnVisibleContentChanged(int32_t index);
    bool SetOnVisibleChanged(const KRRenderCallback &callback);
    bool SetOnScrollCallback(const KRRenderCallback &callback);
    bool SetOnScrollStartCallback(const KRRenderCallback &callback);
    bool SetOnScrollEndCallback(const KRRenderCallback &callback);
    void ScrollToIndex(const std::string &params);
    void UpdateInitialData(const std::string &params);
    void UpdateData(const std::string &params);
    std::shared_ptr<KRRenderValue> GetCommonScrollParams(int32_t index);

    KRRenderCallback ConvertToKCMPCallback(const KRRenderCallback &callback);
};

#endif  // CORE_RENDER_OHOS_KRSYNCSWIPER_H
