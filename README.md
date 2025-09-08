# Softmax-IP
Softmax function implemented in C for vitis HLS

# Notes
Uses axi4-lite interface for communication to use in Vivado block design

Uses pipelines for optimiziation

Designed to be tested with pynq

Currently uses floating point operations

While running synthesis estimated fmax 91.33 MHz 

# To do

Modify to support variable vector size

Create a testbench that compares against cpu or gpu

Maybe switch from floating points to fixed for resource purposes
