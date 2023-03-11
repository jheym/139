#!/bin/bash
# Runs the Application

echo "Enter the buffer size: "
read buffer_size
echo "Enter the number of items to be produced: "
read num_items

echo "Running producer with the following command: ./producer $buffer_size $num_items \$RANDOM"

./producer $buffer_size $num_items $RANDOM