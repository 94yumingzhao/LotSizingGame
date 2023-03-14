\ENCODING=ISO-8859-1
\Problem name: IloCplex

Minimize
 obj1: X_1 + 11 X_2 + 4 X_3 + 7 X_4 + 8 X_5 + 8 X_6 + 100 Y_2 + 132 Y_3
       + 71 Y_4 + 77 Y_5 + 111 Y_6 + 5 I_1 + 3 I_2 + 5 I_3 + 4 I_4 + 2 I_5
       + I_6
Subject To
 c1:  X_1 - I_1  = 15
 c2:  X_2 + I_1 - I_2  = 5
 c3:  X_3 + I_2 - I_3  = 14
 c4:  X_4 + I_3 - I_4  = 9
 c5:  X_5 + I_4 - I_5  = 20
 c6:  X_6 + I_5 - I_6  = 11
 c7:  X_1 - 74 Y_1 <= 0
 c8:  X_2 - 74 Y_2 <= 0
 c9:  X_3 - 74 Y_3 <= 0
 c10: X_4 - 74 Y_4 <= 0
 c11: X_5 - 74 Y_5 <= 0
 c12: X_6 - 74 Y_6 <= 0
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
 0 <= Y_1 <= 1
Binaries
 Y_2  Y_3  Y_4  Y_5  Y_6  Y_1 
Generals
 X_1  X_2  X_3  X_4  X_5  X_6  I_1  I_2  I_3  I_4  I_5  I_6 
End
