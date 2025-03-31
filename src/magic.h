#ifndef MAGIC_H
#define MAGIC_H

/// @brief Opaque type for the MAGIC ADT (Working like a red-black tree)
typedef struct magic *MAGIC;

// Direction enum for mapping queries
enum MAGICDirection {
    STREAM_IN_OUT = 0,  // Map input → output
    STREAM_OUT_IN = 1   // Map output → input
};

/// @brief Initialize a new MAGIC instance.
/// Worst-case time complexity: O(1)
/// @return 
MAGIC MAGICinit();

/// @brief Add 'length' bytes starting from position 'pos'. 
/// Worst-case time complexity: O(log n)
/// @param m MAGIC instance
/// @param pos Starting position
/// @param length Number of bytes to add
void MAGICadd(MAGIC m, int pos, int length);

/// @brief Remove 'length' bytes starting from position 'pos'. 
/// Worst-case time complexity: O(log n)
/// @param m MAGIC instance
/// @param pos Starting position
/// @param length Number of bytes to remove
void MAGICremove(MAGIC m, int pos, int length);

/// @brief Map a position from input to output or vice versa. 
/// Worst-case time complexity: O(log² n)
/// @param m MAGIC instance
/// @param direction Direction of mapping
/// @param pos Position to map
/// @return Mapped position
/// @note If the position is not in the range of the mapping, the result is -1
int MAGICmap(MAGIC m, enum MAGICDirection direction, int pos);

/// @brief Free all resources associated with a MAGIC instance. 
/// Worst-case time complexity: O(n)
/// @param m MAGIC instance
void MAGICdestroy(MAGIC m);

/// @brief Additional function to print the stream for debugging purposes.
/// @param m MAGIC instance
/// @param max_input_pos Maximum input position to print
/// @note This function is not part of the public API and is intended for debugging only.
void MAGICstream(MAGIC m, int max_input_pos);

#endif // MAGIC_H