# faw-encoding
A flag aligned word-based encoding algorithm to compress sorted positive integers. FAW guarantees the upper space complexity O(N). Since FAW-encoded value never exceeds its original position in an array, FAW allows to use the same array to store encoded data. However, most of the existing integers encoding algorithms, such as, WAH, doesn't guarantee the upper space complexity O(N), encoded data cann't be stored in the same array.

# usages

  
  #include "faw_encoding.h"
  ...
  ...
  unsigned int list2_size = 500;
  unsigned int list2[] = {42, 56, 71, 121, 138, 166, 177, 194, 246, ......, 29840, 29894, 29914, 29942, 29958};
  # encode using FAW
  faw_encoded_list2_size = encode(list2, list2_size);  // list2 will contain the encoded data. It can be resized to faw_encoded_list2_size
