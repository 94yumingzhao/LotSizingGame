# SIMM Lot Sizing Cooperative Game
Solving single-item multi-machine lot sizing core allocation cooperative game with column generation and row generation in C++  

About lot sizing:  
https://en.wikipedia.org/wiki/Dynamic_lot-size_model  

About cooperative game:  
https://en.wikipedia.org/wiki/Cooperative_game_theory  

About core:  
https://en.wikipedia.org/wiki/Core_(game_theory)  

About column generation:  
https://en.wikipedia.org/wiki/Column_generation  

Core allocation master problem + Lot sizing sub problem  

++++++++++++++++  column generation master problem ++++++++++++++++  

Minimize  
 obj1: 1393 K_1 + 644 K_2 + 511 K_3 + 482 K_4  
 
Subject To  
 c1: K_1 + K_2 >= 1  
 c2: K_1 + K_3 >= 1  
 c3: K_1 + K_4 >= 1  

++++++++++++++++  row generation master problem  ++++++++++++++++  

Minimize  
 obj1: W_1 + W_2 + W_3  
 
Subject To  
 c1: W_1 + W_2 + W_3  = 1393  
 c2: W_1 <= 644  
 c3: W_2 <= 511  
 c4: W_3 <= 482  
