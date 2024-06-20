#include "sort.h"

static void BubbleSort(bar *bars, u32 length) {
	for (udm i = 0; i < length - 1; ++i) {
		if (i != 0) bars[length - i].color = SORTED;
		for (udm j = 0; j < length - i - 1; ++j) {
			bars[j].color = SORTING;
			bars[j + 1].color = SORTING;
			if (sorting) Sleep(DELAY / length);
			
			if (bars[j].num > bars[j + 1].num) {
				bars[j].color = UNSORTED;
				bars[j + 1].color = UNSORTED;
				if (sorting) Sleep(DELAY / length);
				
				bars[j].num ^= bars[j + 1].num;
				bars[j + 1].num ^= bars[j].num;
				bars[j].num ^= bars[j + 1].num;
				if (sorting) Sleep(DELAY / length);
			}
			
			if (i < length - 2) {
				bars[j].color = INITIAL;
				bars[j + 1].color = INITIAL;
			} else {
				bars[j].color = SORTED;
				bars[j + 1].color = SORTED;
			}
		}
	}
}

static void InsertionSort(bar *bars, u32 length) {
	for (udm i = 0; i < length - 1; ++i) {
		for (udm j = i + 1; j > 0; --j) {
			bars[j - 1].color = SORTING;
			bars[j].color = SORTING;
			if (sorting) Sleep(DELAY / length);
			
			if (bars[j - 1].num > bars[j].num) {
				bars[j - 1].color = UNSORTED;
				bars[j].color = UNSORTED;
				if (sorting) Sleep(DELAY / length);
				
				bars[j - 1].num ^= bars[j].num;
				bars[j].num ^= bars[j - 1].num;
				bars[j - 1].num ^= bars[j].num;
				if (sorting) Sleep(DELAY / length);
				
				bars[j - 1].color = SORTED;
				bars[j].color = SORTED;
				if (sorting) Sleep(DELAY / length);
			} else {
				bars[j - 1].color = SORTED;
				bars[j].color = SORTED;
				break;
			}
		}
	}
}

static void SelectionSort(bar *bars, u32 length) {
	for (udm i = 0; i < length; ++i) {
		udm min = i;
		bars[min].color = SORTING;
		if (sorting) Sleep(DELAY / length);
		for (udm j = i + 1; j < length; ++j) {
			bars[j].color = SORTING;
			if (sorting) Sleep(DELAY / length);
			if (bars[j].num < bars[min].num) {
				bars[min].color = INITIAL;
				min = j;
			} else {
				bars[j].color = INITIAL;
			}
		}
		
		if (min != i) {
			bars[min].color = UNSORTED;
			bars[i].color = UNSORTED;
			if (sorting) Sleep(DELAY / length);
				
			bars[min].num ^= bars[i].num;
			bars[i].num ^= bars[min].num;
			bars[min].num ^= bars[i].num;
			if (sorting) Sleep(DELAY / length);
			
			bars[min].color = INITIAL;
			bars[i].color = SORTED;
			if (sorting) Sleep(DELAY / length);
		} else {
			bars[min].color = SORTED;
			if (sorting) Sleep(DELAY / length);
		}
	}
}

static void MaxHeapify(bar *bars, u32 nodes, u32 parent, u32 length) {
	u32 largest = parent;
	u32 lChild = 2 * parent + 1;
	u32 rChild = 2 * parent + 2;
	
	if ((lChild < nodes) && (bars[parent].num < bars[lChild].num)) largest = lChild;
	if ((rChild < nodes) && (bars[largest].num < bars[rChild].num)) largest = rChild;
	
	if (largest != parent) {
		bars[lChild].color = SORTING;
		if (rChild < nodes) bars[rChild].color = SORTING;
		bars[parent].color = SORTING;
		if (sorting) Sleep(DELAY / length);
		
		bars[largest].color = UNSORTED;
		bars[parent].color = UNSORTED;
		if (sorting) Sleep(DELAY / length);
	
		bars[largest].num ^= bars[parent].num;
		bars[parent].num ^= bars[largest].num;
		bars[largest].num ^= bars[parent].num;
		if (sorting) Sleep(DELAY / length);
		
		bars[lChild].color = INITIAL;
		if (rChild < nodes) bars[rChild].color = INITIAL;
		bars[parent].color = INITIAL;
		if (sorting) Sleep(DELAY / length);
		
		MaxHeapify(bars, nodes, largest, length);
	} else {
		bars[parent].color = INITIAL;
		if (sorting) Sleep(DELAY / length);
	}
}

static void HeapSort(bar *bars, u32 length) {
	for (s32 i = length / 2 - 1; i >= 0; --i) {
		MaxHeapify(bars, length, i, length);
	}
	
	for (u32 i = length - 1; i > 0; --i) {
		bars[0].color = UNSORTED;
		bars[i].color = UNSORTED;
		if (sorting) Sleep(DELAY / length);
		
		bars[0].num ^= bars[i].num;
		bars[i].num ^= bars[0].num;
		bars[0].num ^= bars[i].num;
		if (sorting) Sleep(DELAY / length);
		
		bars[i].color = SORTED;
		if (sorting) Sleep(DELAY / length);
		
		MaxHeapify(bars, i, 0, length);
	}
	
	bars[0].color = SORTED;
}

static void Merge(bar *bars, s32 start, s32 middle, s32 end, u32 length) {
	if (bars[middle].num <= bars[middle + 1].num) return;
	
	for (s32 i = start, j = middle + 1; (j <= end) && (i < j); ++i) {
		u32 value = bars[j].num;
		s32 index = j;
		
		bars[i].color = SORTING;
		bars[index].color = SORTING;
		if (sorting) Sleep(DELAY / length);
		
		if (bars[i].num > bars[index].num) {
			bars[i].color = UNSORTED;
			bars[index].color = UNSORTED;
			if (sorting) Sleep(DELAY / length);
			
			while (index != i) {
				bars[index].num = bars[index - 1].num;
				index--;
			}
			
			bars[i].num = value;
			if (sorting) Sleep(DELAY / length);
			
			bars[i].color = INITIAL;
			bars[j].color = INITIAL;
			if (sorting) Sleep(DELAY / length);
			
			j++;
		} else {
			bars[i].color = INITIAL;
			bars[index].color = INITIAL;
			if (sorting) Sleep(DELAY / length);
		}
	}
}

static void Divide(bar *bars, s32 start, s32 end, u32 length) {
	if ((end - start) < 1) return;
	
	s32 middle = (end + start) / 2;
	Divide(bars, start, middle, length);
	Divide(bars, middle + 1, end, length);
	Merge(bars, start, middle, end, length);
}

static void MergeSort(bar *bars, u32 length) {
	Divide(bars, 0, length - 1, length);
	for (u32 i = 0; i < length; ++i) {
		bars[i].color = SORTED;
	}
}

static u32 Partition(bar *bars, s32 low, s32 high, u64 seed, u32 length) {
	u32 pivot = (u32) (BOGRandom(&seed) % (high - low + 1) + low); // randomized pivot
	
	bars[pivot].color = SORTING;
	if (sorting) Sleep(DELAY / length);
	
	if (pivot != (u32) high) {
		bars[pivot].color = UNSORTED;
		bars[high].color = UNSORTED;
		if (sorting) Sleep(DELAY / length);
		
		bars[pivot].num ^= bars[high].num;
		bars[high].num ^= bars[pivot].num;
		bars[pivot].num ^= bars[high].num;
		if (sorting) Sleep(DELAY / length);
		
		bars[pivot].color = INITIAL;
		bars[high].color = SORTING;
		if (sorting) Sleep(DELAY / length);
	}
	
	u32 median = low;
	for (udm i = low; i < high; ++i) {
		bars[i].color = SORTING;
		if (sorting) Sleep(DELAY / length);
		
		if(bars[i].num < bars[high].num) {
			if (i == median) {
				bars[i].color = INITIAL;
				median++;
				continue;
			}
			
			bars[i].color = UNSORTED;
			bars[median].color = UNSORTED;
			if (sorting) Sleep(DELAY / length);
			
			bars[i].num ^= bars[median].num;
			bars[median].num ^= bars[i].num;
			bars[i].num ^= bars[median].num;
			if (sorting) Sleep(DELAY / length);
			
			bars[i].color = INITIAL;
			bars[median++].color = INITIAL;
			if (sorting) Sleep(DELAY / length);
		}
		
		bars[i].color = INITIAL;
	}
	
	if (median != (u32) high) {
		bars[median].color = UNSORTED;
		bars[high].color = UNSORTED;
		if (sorting) Sleep(DELAY / length);
		
		bars[high].num ^= bars[median].num;
		bars[median].num ^= bars[high].num;
		bars[high].num ^= bars[median].num;
		if (sorting) Sleep(DELAY / length);
		
		bars[median].color = SORTING;
		bars[high].color = INITIAL;
		if (sorting) Sleep(DELAY / length);
	}
	
	bars[median].color = INITIAL;
	
	return median;
}

static void Sort(bar *bars, s32 low, s32 high, u64 seed, u32 length) {
	if ((high - low) <= 0) return;
	
	u32 pivot = Partition(bars, low, high, seed, length);
	Sort(bars, low, pivot - 1, seed, length);
	Sort(bars, pivot + 1, high, seed, length);
}

static void QuickSort(bar *bars, u32 length) {
	u64 seed = BOGGetSystemTime();
	Sort(bars, 0, length - 1, seed, length);
	for (u32 i = 0; i < length; ++i) {
		bars[i].color = SORTED;
	}
}
