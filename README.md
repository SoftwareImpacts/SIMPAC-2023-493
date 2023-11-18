# FAW-encoding
A flag aligned word-based encoding algorithm to compress sorted positive integers. FAW guarantees the upper space complexity O(N). Since FAW-encoded value never exceeds its original position in an array, FAW allows to use the same array to store encoded data. However, most of the existing integers encoding algorithms, such as, WAH, doesn't guarantee the upper space complexity O(N), encoded data cann't be stored in the same array.

# Usages

  
  #include "faw_encoding.h"
  
  ...
  
  ...
  
  unsigned int list2_size = 500;
  
  unsigned int list2[] = {42, 56, 71, 121, 138, 166, 177, 194, 246, ......, 29840, 29894, 29914, 29942, 29958};
  
  ## Encode using FAW
  faw_encoded_list2_size = encode(list2, list2_size);  /** list2 will contain the encoded data. It can be resized to faw_encoded_list2_size **/

  ## Show items from encoded data
  show_items(list2, faw_encoded_list2_size);

  ## Decode
  list2_size = decode(list2, faw_encoded_list2_size);

  ## Search an item in FAW-encoded data
  unsigned int needle = 1232;
  result = exists(list2, faw_encoded_list2_size, needle);  /** if result==1, needle exists **/

  ## Intersect: Find common values in two FAW-encoded data
  unsigned int result_size = 0;      /** store the result size **/
  unsigned int* results = intersect(list1, faw_encoded_list1_size, list2, faw_encoded_list2_size, &result_size);

  ## Union: Merged values from two FAW-encoded data
  unsigned int result_size = 0;      /** store the result size **/
  unsigned int* results = merge(list1, faw_encoded_list1_size, list2, faw_encoded_list2_size, &result_size);

  
