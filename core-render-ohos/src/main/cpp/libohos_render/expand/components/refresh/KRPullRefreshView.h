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

#ifndef CORE_RENDER_OHOS_KRPULLREFRESHVIEW_H
#define CORE_RENDER_OHOS_KRPULLREFRESHVIEW_H

#include "libohos_render/export/IKRRenderViewExport.h"
#include "libohos_render/utils/animate/KRAnimation.h"

class KRPullRefreshView : public IKRRenderViewExport {
public:
    KRPullRefreshView() = default;
    KRPullRefreshView(const KRPullRefreshView &) = delete;
    KRPullRefreshView(KRPullRefreshView &&) = delete;
    KRPullRefreshView &operator=(const KRPullRefreshView &) = delete;
    KRPullRefreshView &operator=(KRPullRefreshView &&) = delete;

    ArkUI_NodeHandle CreateNode() override;
    bool ReuseEnable() override;
    void OnEvent(ArkUI_NodeEvent *event, const ArkUI_NodeEventType &event_type) override;
    bool SetProp(const std::string &prop_key, const KRAnyValue &prop_value,
                 const KRRenderCallback event_call_back = nullptr) override;
    bool ResetProp(const std::string &prop_key) override;
    void DidInit() override;
    void DidInsertSubRenderView(const std::shared_ptr<IKRRenderViewExport> &sub_render_view, int index) override;
    void CallMethod(const std::string &method, const KRAnyValue &params, const KRRenderCallback &callback) override;

private:
    ArkUI_NodeHandle ark_ui_node_handler_ = nullptr;
    std::shared_ptr<IKRRenderViewExport> loading_view_export_;
    std::shared_ptr<IKRRenderViewExport> content_view_export_;
    bool is_refreshing_ = false;
    int hide_generation_ = 0;
    bool is_horizontal_slide_ = false;
    bool is_direction_locked_ = false;
    float start_point_x_ = 0.0f;
    float start_point_y_ = 0.0f;
    float last_point_y_ = 0.0f;
    float current_offset_y_ = 0.0f;
    float loading_x_ = 0.0f;
    float loading_height_ = 0.0f;
    KRRenderCallback loading_show_callback_;
    std::shared_ptr<KRAnimation> content_inset_animate_;
    std::shared_ptr<KRAnimation> hide_animate_;

    void ResetGestureState();
    void ProcessTouchEvent(ArkUI_NodeEvent *event);
    void OnTouchDownEvent(ArkUI_UIInputEvent *input_event);
    void OnTouchMoveEvent(ArkUI_UIInputEvent *input_event);
    void OnTouchUpEvent(ArkUI_UIInputEvent *input_event);
    void OnTouchCancelEvent(ArkUI_UIInputEvent *input_event);
    void HideLoading();
    void FinishSpinner();
    void UpdateLoading(ArkUI_UIInputEvent *input_event);
    void SetPosition(ArkUI_NodeHandle node, float x, float y);
    void ReboundToLoadingHeight();
    void SetLayoutInfo(const std::string &info);
    void EnableContentScroll(bool enable);
};

#endif  // CORE_RENDER_OHOS_KRPULLREFRESHVIEW_H
