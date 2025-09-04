# 数组操作插件 (Array Plugin)

## 概述
数组操作插件提供了丰富的数组操作功能，包括数组创建、修改、搜索、排序等操作。

## 功能列表
- **array_create**: 创建数组
- **array_push**: 向数组末尾添加元素
- **array_pop**: 从数组末尾移除元素
- **array_insert**: 在指定位置插入元素
- **array_remove**: 移除指定位置的元素
- **array_find**: 查找元素在数组中的位置
- **array_sort**: 数组排序
- **array_reverse**: 数组反转
- **array_slice**: 数组切片
- **array_join**: 数组连接

## 使用方法
```cpp
// 创建数组
let arr = array_create([1, 2, 3, 4, 5]);

// 数组操作
array_push(arr, 6);
let last = array_pop(arr);
array_insert(arr, 2, 10);
array_remove(arr, 1);

// 数组搜索和排序
let index = array_find(arr, 3);
array_sort(arr);
array_reverse(arr);

// 数组切片和连接
let slice = array_slice(arr, 1, 3);
let newArr = array_join(arr, slice);
```

## 构建
插件会自动构建为 `libarray_plugin.so` 并输出到 `build/plugins/` 目录。
