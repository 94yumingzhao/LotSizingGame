\ENCODING=ISO-8859-1
\Problem name: IloCplex

Minimize
 obj1: V
Subject To
 c1: W_1 + W_2 + W_3  = 1393
 c2: - V + W_1 <= 644
 c3: - V + W_2 <= 511
 c4: - V + W_3 <= 482
 c5: - V + W_1 + W_2 <= 1029
 c6: - V + W_1 + W_2 <= 1003
Bounds
      V >= 0
      W_1 >= 0
      W_2 >= 0
      W_3 >= 0
Generals
 V  W_1  W_2  W_3 
End
