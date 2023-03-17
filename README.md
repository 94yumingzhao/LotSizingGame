# SIMM Lot Sizing Cooperative Game
Solving single-item multi-machine lot sizing core allocation cooperative game with column generation and row generation in C++  

Core allocation master problem + Lot sizing sub problem  

++++++++++++++++  Solving with column generation in GMLSCG file ++++++++++++++++  

Minimize  
 obj1: 1393 K_1 + 644 K_2 + 511 K_3 + 482 K_4  
 
Subject To  
 c1: K_1 + K_2 >= 1  
 c2: K_1 + K_3 >= 1  
 c3: K_1 + K_4 >= 1  

++++++++++++++++  Solving with row generation in GMLSRG file  ++++++++++++++++  
