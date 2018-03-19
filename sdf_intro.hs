module SDF_Intro where

import ForSyDe.Shallow

-- Netlist
system s_in = (s_2, s_out) where
    s_1   = p_add s_in s_2
    s_2   = p_delay s_1
    s_out = p_average s_2   

-- Process specification
p_add s1 s2 = actor21SDF (1,1) 1 add s1 s2
p_delay s   = delaySDF 0 s
p_average s = actor11SDF 3 1 average s

-- Function definition
add [x] [y] = [x + y]
average [x1,x2,x3] = [(x1 + x2 + x3) / 3.0]

-- Test Signal
s_test = signal [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0]


    
    