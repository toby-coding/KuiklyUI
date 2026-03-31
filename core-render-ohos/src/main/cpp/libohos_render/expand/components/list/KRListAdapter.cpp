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

#include "libohos_render/expand/components/list/KRListAdapter.h"
#include "libohos_render/foundation/thread/KRMainThread.h"
#include "libohos_render/utils/KRRenderLoger.h"
#include "libohos_render/utils/KRViewUtil.h"

static constexpr const char *INDEX = "index";
static constexpr const char *REUSE_INDEX = "reuseIndex";

KRListAdapter::KRListAdapter() : handle_(OH_ArkUI_NodeAdapter_Create()) {
    OH_ArkUI_NodeAdapter_RegisterEventReceiver(handle_, this, OnStaticAdapterEvent);
}

KRListAdapter::~KRListAdapter() {
    if (KRISMainThread()) {
        Dispose();
    }
}

void KRListAdapter::setOnBindCallback(KRRenderCallback callback) {
    on_bind_call_back_ = callback;
}

ArkUI_NodeAdapterHandle KRListAdapter::GetHandle() const {
    return handle_;
}

std::string KRListAdapter::MakeItemUniqueValue(int32_t value) {
    return std::to_string(insert_update_times_) + "-" + std::to_string(value);
}

void KRListAdapter::SetInitialCount(int32_t size) {
    for (int32_t i = 0; i < size; i++) {
        list_item_data_array_.emplace_back(std::make_shared<KRListItemData>(i));
    }
    insert_update_times_++;
    OH_ArkUI_NodeAdapter_SetTotalNodeCount(handle_, list_item_data_array_.size());
    OH_ArkUI_NodeAdapter_ReloadAllItems(handle_);
}

void KRListAdapter::RemoveItem(int32_t index) {
    list_item_data_array_.erase(list_item_data_array_.begin() + index);
    OH_ArkUI_NodeAdapter_RemoveItem(handle_, index, 1);
    OH_ArkUI_NodeAdapter_SetTotalNodeCount(handle_, list_item_data_array_.size());
    cached_items_[index] = nullptr;
}

void KRListAdapter::InsertItem(int32_t index, int32_t count) {
    for (int i = 0; i < count; i++) {
        list_item_data_array_.insert(list_item_data_array_.begin() + index + i,
                                     std::make_shared<KRListItemData>(index + i));
    }
    insert_update_times_++;
    OH_ArkUI_NodeAdapter_InsertItem(handle_, index, count);
    OH_ArkUI_NodeAdapter_SetTotalNodeCount(handle_, list_item_data_array_.size());
}

void KRListAdapter::ReloadItem(int32_t index) {
    OH_ArkUI_NodeAdapter_ReloadItem(handle_, index, 1);
}

void KRListAdapter::InsertChildAt(ArkUI_NodeHandle handle, int index) {
    children_[index] = handle;
}

void KRListAdapter::RemoveChildAt(int index) {
    children_[index] = nullptr;
}

ArkUI_NodeHandle KRListAdapter::GetArkUIChildHandle(int index) {
    return children_[index];
}

std::vector<std::shared_ptr<KRListItemData>> KRListAdapter::getListItemDataArray() {
    return list_item_data_array_;
}

void KRListAdapter::OnStaticAdapterEvent(ArkUI_NodeAdapterEvent *event) {
    auto itemAdapter = reinterpret_cast<KRListAdapter *>(OH_ArkUI_NodeAdapterEvent_GetUserData(event));
    itemAdapter->OnAdapterEvent(event);
}

void KRListAdapter::OnAdapterEvent(ArkUI_NodeAdapterEvent *event) {
    auto type = OH_ArkUI_NodeAdapterEvent_GetType(event);
    switch (type) {
    case NODE_ADAPTER_EVENT_ON_GET_NODE_ID:
        OnNewItemIdCreated(event);
        break;
    case NODE_ADAPTER_EVENT_ON_ADD_NODE_TO_ADAPTER:
        OnNewItemAttached(event);
        break;
    case NODE_ADAPTER_EVENT_ON_REMOVE_NODE_FROM_ADAPTER:
        OnItemDetached(event);
        break;
    default:
        break;
    }
}

void KRListAdapter::OnNewItemIdCreated(ArkUI_NodeAdapterEvent *event) {
    auto index = OH_ArkUI_NodeAdapterEvent_GetItemIndex(event);
    if (index < 0 || index >= static_cast<int32_t>(list_item_data_array_.size())) {
        KR_LOG_ERROR << "Invalid index in OnNewItemIdCreated: " << index;
        return;
    }
    static std::hash<std::string> hashId = std::hash<std::string>();
    auto id = hashId(MakeItemUniqueValue(list_item_data_array_[index]->getPosition()));
    OH_ArkUI_NodeAdapterEvent_SetNodeId(event, id);
}

void KRListAdapter::SetFreezingItems(std::vector<int> item_indices) {
    freezing_item_indices_ = item_indices;
}

void KRListAdapter::OnNewItemAttached(ArkUI_NodeAdapterEvent *event) {
    int32_t index = OH_ArkUI_NodeAdapterEvent_GetItemIndex(event);
    ArkUI_NodeHandle item_handle = nullptr;
    std::unordered_map<std::string, KRAnyValue> map;
    map[INDEX] = NewKRRenderValue(index);

    std::shared_ptr<KRListItem> recycled_item = freezing_cached_items_[index];
    bool is_freezing_item = false;

    if (recycled_item) {
        is_freezing_item = true;
    } else {
        recycled_item = cached_items_[index];
    }

    if (recycled_item) {
        item_handle = recycled_item->getNodeHandle();

        if (!is_freezing_item) {
            KR_LOG_INFO << "KRListAdapter OnNewItemAttached reuse index:" << index;
            ArkUI_NodeHandle current_child_handler = kuikly::util::GetNodeApi()->getChildAt(item_handle, 0);
            int32_t reusePosition = recycled_item->getPosition();

            map[REUSE_INDEX] = NewKRRenderValue(reusePosition);
            KRAnyValue message = NewKRRenderValue(map);
            if (on_bind_call_back_) {
                on_bind_call_back_(message);
            }

            ArkUI_NodeHandle child_node_handle = GetArkUIChildHandle(index);
            auto node_api = kuikly::util::GetNodeApi();

            if (child_node_handle != current_child_handler) {
                if (current_child_handler && node_api->IsNodeAlive(current_child_handler)) {
                    node_api->removeChild(item_handle, current_child_handler);
                }
                if (child_node_handle && node_api->IsNodeAlive(child_node_handle)) {
                    node_api->insertChildAt(item_handle, child_node_handle, 0);
                    node_api->resetAttribute(child_node_handle, NODE_POSITION);
                }
            } else {
                if (child_node_handle && node_api->IsNodeAlive(child_node_handle)) {
                    node_api->resetAttribute(child_node_handle, NODE_POSITION);
                }
            }

            cached_items_[index] = nullptr;
            items_[item_handle] = recycled_item;
            recycled_item->setListItemData(list_item_data_array_[index]);
            item_handle_to_index_[item_handle] = index;
        } else {
            freezing_cached_items_[index] = nullptr;
            KR_LOG_INFO << "KRListAdapter OnNewItemAttached reuse freezing_item index:" << index;
        }
    } else {
        KR_LOG_INFO << "KRListAdapter OnNewItemAttached new index:" << index;
        item_handle = kuikly::util::GetNodeApi()->createNode(ARKUI_NODE_LIST_ITEM);
        auto list_item = std::make_shared<KRListItem>(item_handle);
        list_item->setListItemData(list_item_data_array_[index]);

        std::unordered_map<std::string, KRAnyValue> map;
        map[INDEX] = NewKRRenderValue(index);
        KRAnyValue message = NewKRRenderValue(map);

        if (on_bind_call_back_) {
            on_bind_call_back_(message);
        }

        ArkUI_NodeHandle child_node_handle = GetArkUIChildHandle(index);
        auto node_api = kuikly::util::GetNodeApi();

        if (child_node_handle && node_api->IsNodeAlive(child_node_handle)) {
            node_api->insertChildAt(item_handle, child_node_handle, 0);
            node_api->resetAttribute(child_node_handle, NODE_POSITION);
        }

        items_.emplace(item_handle, list_item);
        item_handle_to_index_[item_handle] = index;
    }

    OH_ArkUI_NodeAdapterEvent_SetItem(event, item_handle);
}

void KRListAdapter::OnItemDetached(ArkUI_NodeAdapterEvent *event) {
    auto handler = OH_ArkUI_NodeAdapterEvent_GetRemovedNode(event);
    auto item = items_[handler];
    int position = item->getPosition();
    if (position == item->getCreateViewPosition()) {
        if (IsFreezingItem(position)) {
            freezing_cached_items_[position] = item;
        } else {
            cached_items_[position] = item;
        }
    }
    item_handle_to_index_.erase(handler);
}

bool KRListAdapter::IsFreezingItem(int index) {
    return std::find(freezing_item_indices_.begin(), freezing_item_indices_.end(), index) !=
           freezing_item_indices_.end();
}

void KRListAdapter::BatchUpdateComplete() {
    for (size_t i = 0; i < list_item_data_array_.size(); ++i) {
        auto data = list_item_data_array_[i];
        if (data) {
            int old_index = data->getPosition();
            data->setPosition(i);
            if (old_index != static_cast<int>(i)) {
                OnItemIndexChanged(i, old_index);
            }
        }
    }
}

void KRListAdapter::OnItemIndexChanged(int new_index, int old_index) {
    if (new_index == old_index) return;
    auto recycled_item = cached_items_[old_index];
    if (recycled_item) {
        cached_items_[old_index] = nullptr;
    }
}

void KRListAdapter::Dispose() {
    items_.clear();
    OH_ArkUI_NodeAdapter_UnregisterEventReceiver(handle_);
    OH_ArkUI_NodeAdapter_Dispose(handle_);
}

void KRListAdapter::ForceRelayout() {
    auto node_api = kuikly::util::GetNodeApi();
    for (auto &pair : item_handle_to_index_) {
        auto item_handle = pair.first;
        if (item_handle && node_api->IsNodeAlive(item_handle)) {
            ArkUI_NodeHandle child_node_handle = node_api->getChildAt(item_handle, 0);
            if (child_node_handle && node_api->IsNodeAlive(child_node_handle)) {
                node_api->resetAttribute(child_node_handle, NODE_POSITION);
            }
        }
    }
}

ArkUI_NodeHandle KRListAdapter::GetItemNodeHandle(int32_t index) {
    for (auto &pair : items_) {
        auto item = pair.second;
        if (item && item->getPosition() == index) {
            return item->getNodeHandle();
        }
    }
    return nullptr;
}
