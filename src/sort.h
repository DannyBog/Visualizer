#ifndef SORT_H
#define SORT_H

typedef struct {
	u32 num;
	COLORREF color;
} bar;

#define INITIAL		0xfff000
#define UNSORTED	0x0000ff
#define SORTED		0x00ff00
#define SORTING		0x00ffff

#define MAX_BARS	4096
#define DELAY		2000

static bool sorting;

static void BubbleSort(bar *bars, u32 length);
static void InsertionSort(bar *bars, u32 length);
static void SelectionSort(bar *bars, u32 length);

static void MaxHeapify(bar *bars, u32 nodes, u32 parent, u32 length);
static void HeapSort(bar *bars, u32 length);

static void Merge(bar *bars, s32 start, s32 middle, s32 end, u32 length);
static void Divide(bar *bars, s32 start, s32 end, u32 length);
static void MergeSort(bar *bars, u32 length);

static u32 Partition(bar *bars, s32 low, s32 high, u64 seed, u32 length);
static void Sort(bar *bars, s32 low, s32 high, u64 seed, u32 length);
static void QuickSort(bar *bars, u32 length);

#endif //SORT_H
