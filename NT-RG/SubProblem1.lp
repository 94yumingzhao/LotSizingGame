\ENCODING=ISO-8859-1
\Problem name: IloCplex

Maximize
 obj1: - X_1 - 11 X_2 - 4 X_3 - 7 X_4 - 8 X_5 - 8 X_6 - 100 Y_2 - 132 Y_3
       - 71 Y_4 - 77 Y_5 - 111 Y_6 - 5 I_1 - 3 I_2 - 5 I_3 - 4 I_4 - 2 I_5
       - I_6 + 420 Z_1 + 330 Z_2 + 249 Z_3
Subject To
 c1:  X_1 - I_1 - 15 Z_1 - Z_2 - 20 Z_3  = 0
 c2:  X_2 + I_1 - I_2 - 5 Z_1 - Z_2 - 8 Z_3  = 0
 c3:  X_3 + I_2 - I_3 - 14 Z_1 - 11 Z_2 - Z_3  = 0
 c4:  X_4 + I_3 - I_4 - 9 Z_1 - 17 Z_2 - 11 Z_3  = 0
 c5:  X_5 + I_4 - I_5 - 20 Z_1 - 3 Z_2 - Z_3  = 0
 c6:  X_6 + I_5 - I_6 - 11 Z_1 - 14 Z_2 - 19 Z_3  = 0
 c7:  X_1 - 1000 Y_1 <= 0
 c8:  X_2 - 1000 Y_2 <= 0
 c9:  X_3 - 1000 Y_3 <= 0
 c10: X_4 - 1000 Y_4 <= 0
 c11: X_5 - 1000 Y_5 <= 0
 c12: X_6 - 1000 Y_6 <= 0
Bounds
      X_1 >= 0
      X_2 >= 0
      X_3 >= 0
      X_4 >= 0
      X_5 >= 0
      X_6 >= 0
 0 <= Y_2 <= 1
 0 <= Y_3 <= 1
 0 <= Y_4 <= 1
 0 <= Y_5 <= 1
 0 <= Y_6 <= 1
      I_1 >= 0
      I_2 >= 0
      I_3 >= 0
      I_4 >= 0
      I_5 >= 0
      I_6 >= 0
 0 <= Z_1 <= 1
 0 <= Z_2 <= 1
 0 <= Z_3 <= 1
 0 <= Y_1 <= 1
Binaries
 Y_2  Y_3  Y_4  Y_5  Y_6  Z_1  Z_2  Z_3  Y_1 
Generals
 X_1  X_2  X_3  X_4  X_5  X_6  I_1  I_2  I_3  I_4  I_5  I_6 
End
