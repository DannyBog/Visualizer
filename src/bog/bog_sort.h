#ifndef BOG_SORT_H
#define BOG_SORT_H

static void BOGBubbleSort(int32_t *arr, size_t length) {
	for (size_t i = 0; i < length - 1; ++i) {
		for (size_t j = 0; j < length - i - 1; ++j) {
			if (arr[j] > arr[j + 1]) {
				arr[j] ^= arr[j + 1];
				arr[j + 1] ^= arr[j];
				arr[j] ^= arr[j + 1];
			}
		}
	}
}

static void BOGInsertionSort(int32_t *arr, size_t length) {
	for (size_t i = 0; i < length - 1; ++i) {
		for (size_t j = i + 1; j > 0; --j) {
			if (arr[j - 1] > arr[j]) {
				arr[j - 1] ^= arr[j];
				arr[j] ^= arr[j - 1];
				arr[j - 1] ^= arr[j];
			}
		}
	}
}

#endif //BOG_SORT_H
