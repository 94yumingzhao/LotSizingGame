\ENCODING=ISO-8859-1
\Problem name: IloCplex

Minimize
 obj1: W_1 + W_2 + W_3
Subject To
 c1: W_1 + W_2 + W_3  = 1393
 c2: W_1 <= 644
 c3: W_2 <= 511
 c4: W_3 <= 482
 c5: W_1 + W_2 <= 1029
 c6: W_1 + W_3 <= 1003
Bounds
      W_1 >= 0
      W_2 >= 0
      W_3 >= 0
Generals
 W_1  W_2  W_3 
End
