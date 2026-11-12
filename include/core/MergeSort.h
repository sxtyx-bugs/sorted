#pragma once

#include <vector>
#include <functional>
#include <cstddef>
#include "../models/FileRecord.h"

namespace Core {

/**
 * @brief DAA Implementation of Merge Sort (Divide and Conquer).
 * Recurrence: T(n) = 2T(n/2) + O(n)
 * Time Complexity: O(n log n) Best, Average, Worst.
 * Auxiliary Space: O(n) for merging buffers.
 * Stable: Yes, strict weak ordering comp(a, b) maintains original order when !comp(a,b) && !comp(b,a).
 */
template <typename T, typename Compare>
void merge(std::vector<T>& arr, std::size_t left, std::size_t mid, std::size_t right, Compare comp) {
    std::size_t n1 = mid - left + 1;
    std::size_t n2 = right - mid;

    std::vector<T> L;
    L.reserve(n1);
    for (std::size_t i = 0; i < n1; ++i) {
        L.push_back(arr[left + i]);
    }

    std::vector<T> R;
    R.reserve(n2);
    for (std::size_t j = 0; j < n2; ++j) {
        R.push_back(arr[mid + 1 + j]);
    }

    std::size_t i = 0;
    std::size_t j = 0;
    std::size_t k = left;

    while (i < n1 && j < n2) {
        // Stable: if equal, take from left subarray
        if (comp(L[i], R[j])) {
            arr[k++] = std::move(L[i++]);
        } else if (comp(R[j], L[i])) {
            arr[k++] = std::move(R[j++]);
        } else {
            // Equivalent elements preserve relative order
            arr[k++] = std::move(L[i++]);
        }
    }

    while (i < n1) {
        arr[k++] = std::move(L[i++]);
    }

    while (j < n2) {
        arr[k++] = std::move(R[j++]);
    }
}

template <typename T, typename Compare>
void mergeSortInternal(std::vector<T>& arr, std::size_t left, std::size_t right, Compare comp) {
    if (left >= right) {
        return;
    }

    std::size_t mid = left + (right - left) / 2;
    mergeSortInternal(arr, left, mid, comp);
    mergeSortInternal(arr, mid + 1, right, comp);
    merge(arr, left, mid, right, comp);
}

template <typename T, typename Compare>
void mergeSort(std::vector<T>& arr, Compare comp) {
    if (arr.size() <= 1) {
        return;
    }
    mergeSortInternal(arr, 0, arr.size() - 1, comp);
}

// Convenience sort key enumerations and comparators for FileRecord
enum class SortKey {
    NameAsc,
    NameDesc,
    SizeAsc,
    SizeDesc,
    ExtensionAsc,
    ExtensionDesc,
    DateAsc,
    DateDesc,
    CategoryAsc,
    CategoryDesc
};

inline void sortFileRecords(std::vector<FileRecord>& records, SortKey key) {
    switch (key) {
        case SortKey::NameAsc:
            mergeSort(records, [](const FileRecord& a, const FileRecord& b) {
                return a.name < b.name;
            });
            break;
        case SortKey::NameDesc:
            mergeSort(records, [](const FileRecord& a, const FileRecord& b) {
                return a.name > b.name;
            });
            break;
        case SortKey::SizeAsc:
            mergeSort(records, [](const FileRecord& a, const FileRecord& b) {
                return a.size < b.size;
            });
            break;
        case SortKey::SizeDesc:
            mergeSort(records, [](const FileRecord& a, const FileRecord& b) {
                return a.size > b.size;
            });
            break;
        case SortKey::ExtensionAsc:
            mergeSort(records, [](const FileRecord& a, const FileRecord& b) {
                return a.extension < b.extension;
            });
            break;
        case SortKey::ExtensionDesc:
            mergeSort(records, [](const FileRecord& a, const FileRecord& b) {
                return a.extension > b.extension;
            });
            break;
        case SortKey::DateAsc:
            mergeSort(records, [](const FileRecord& a, const FileRecord& b) {
                return a.modifiedTime < b.modifiedTime;
            });
            break;
        case SortKey::DateDesc:
            mergeSort(records, [](const FileRecord& a, const FileRecord& b) {
                return a.modifiedTime > b.modifiedTime;
            });
            break;
        case SortKey::CategoryAsc:
            mergeSort(records, [](const FileRecord& a, const FileRecord& b) {
                return a.category < b.category;
            });
            break;
        case SortKey::CategoryDesc:
            mergeSort(records, [](const FileRecord& a, const FileRecord& b) {
                return a.category > b.category;
            });
            break;
    }
}

} // namespace Core
