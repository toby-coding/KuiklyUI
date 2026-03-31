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

#ifndef CORE_RENDER_OHOS_KRLISTADAPTER_H
#define CORE_RENDER_OHOS_KRLISTADAPTER_H

#include <arkui/native_node.h>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <memory>
#include "libohos_render/foundation/KRCommon.h"

class KRListItemData {
public:
    explicit KRListItemData(int32_t position) : position_(position), create_view_position_(position) {}

    void setPosition(int32_t index) { position_ = index; }
    int32_t getPosition() { return position_; }
    int32_t getCreateViewPosition() { return create_view_position_; }

private:
    int32_t position_;
    int32_t create_view_position_;
};

class KRListItem {
public:
    explicit KRListItem(ArkUI_NodeHandle node_handle) : node_handle_(node_handle) {}

    ArkUI_NodeHandle getNodeHandle() { return node_handle_; }

    void setListItemData(std::shared_ptr<KRListItemData> data) { list_item_data_ = data; }
    std::shared_ptr<KRListItemData> getListItemData() { return list_item_data_; }
    int32_t getPosition() { return list_item_data_ ? list_item_data_->getPosition() : -1; }
    int32_t getCreateViewPosition() { return list_item_data_ ? list_item_data_->getCreateViewPosition() : -1; }

private:
    ArkUI_NodeHandle node_handle_ = nullptr;
    std::shared_ptr<KRListItemData> list_item_data_;
};

class KRListAdapter {
public:
    KRListAdapter();
    ~KRListAdapter();

    void setOnBindCallback(KRRenderCallback callback);
    ArkUI_NodeAdapterHandle GetHandle() const;
    void SetInitialCount(int32_t size);
    std::vector<std::shared_ptr<KRListItemData>> getListItemDataArray();
    void RemoveItem(int32_t index);
    void InsertItem(int32_t index, int32_t count);
    void ReloadItem(int32_t index);
    void InsertChildAt(ArkUI_NodeHandle handle, int index);
    void RemoveChildAt(int index);
    void BatchUpdateComplete();
    void SetFreezingItems(std::vector<int> item_indices);
    void ForceRelayout();
    ArkUI_NodeHandle GetItemNodeHandle(int32_t index);

private:
    std::vector<std::shared_ptr<KRListItemData>> list_item_data_array_;
    ArkUI_NodeAdapterHandle handle_ = nullptr;
    std::unordered_map<ArkUI_NodeHandle, std::shared_ptr<KRListItem>> items_;
    std::unordered_map<int, std::shared_ptr<KRListItem>> cached_items_;
    std::unordered_map<int, std::shared_ptr<KRListItem>> freezing_cached_items_;
    KRRenderCallback on_bind_call_back_;
    int32_t insert_update_times_ = 0;
    std::map<int, ArkUI_NodeHandle> children_;
    std::vector<int> freezing_item_indices_;
    std::unordered_map<ArkUI_NodeHandle, int> item_handle_to_index_;

    std::string MakeItemUniqueValue(int32_t value);
    ArkUI_NodeHandle GetArkUIChildHandle(int index);
    static void OnStaticAdapterEvent(ArkUI_NodeAdapterEvent *event);
    void OnAdapterEvent(ArkUI_NodeAdapterEvent *event);
    void OnNewItemIdCreated(ArkUI_NodeAdapterEvent *event);
    void OnNewItemAttached(ArkUI_NodeAdapterEvent *event);
    void OnItemDetached(ArkUI_NodeAdapterEvent *event);
    void OnItemIndexChanged(int new_index, int old_index);
    bool IsFreezingItem(int index);
    void Dispose();
};

#endif  // CORE_RENDER_OHOS_KRLISTADAPTER_H
