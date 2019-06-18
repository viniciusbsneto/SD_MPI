"""
Nome: Vinícius Barreto de Sousa Neto

Entrada: N.A.
Saída: Aproximação da integral no intervalo [a, b] da função f(x)
       utilizando o método do trapézio simples com n subdivisões.

Algoritmo:
1.    Cada processo calcula sua partição local do intervalo [a, b]

2.    Cada processo calcula a aproximação da integral conforme sua partição
      local do intervalo [a, b].
      
3a.   Cada processo de rank != 0 envia sua aproximação da integral para o
      processo de rank 0.
      
3b.   O processo de rank 0 soma as integrais "parciais" enviadas a ele por
      cada outro processo e exibe o resultado.

Obs.: n/p deve ser exato. O número de subdivisões deve ser igual para cada processo.

int    my_rank   Rank do processo
int    p         Número de processos
float  a         Limite inferior da integral
float  b         Limite superior da integral
int    n         Número de subdivisões (trapézios)
float  h         Tamanho da base do trapézio (tamanho da subdivisão)
float  local_a   Limite inferior da partição local do intervalo em um processo
float  local_b   Limite superior da partição local do intervalo em um processo
int    local_n   Número de subdivisões (trapézios) para cálculo local
float  integral  Integral no intervalo
float  total     Integral total
int    source    Processo de origem
int    dest      Processo de destino
"""

def f(x):
    return 4/(1+x*x)

def trap(a, b, n, h):

    integral = (f(a) + f(b))/2.0
    
    x = a
    
    for i in range(1, int(n)):
        x = x + h
        integral = integral + f(x)
        
    return integral * h

from mpi4py import MPI

comm = MPI.COMM_WORLD
my_rank = comm.Get_rank()
p = comm.Get_size()

a = 0.0
b = 1.0
n = 1024
dest = 0
total = -1.0

h = (b-a)/n
local_n = n/p

local_a = a + my_rank * local_n * h
local_b = local_a + local_n * h
integral = trap(local_a, local_b, local_n, h)

if my_rank == 0:
    
    total = integral 
    for source in range(1, p):
        integral = comm.recv(source=source)
        print("PE ", my_rank, "<-", source, ",", integral, "\n")
        total = total + integral 
        
else:
    
    print("PE ", my_rank, "->", dest, ",", integral, "\n")
    comm.send(integral, dest=0)
    
if my_rank == 0:
    
    print("Para n =", n, "subdivisoes (trapezios): \n")
    print("Integral no intervalo [",a,",", b, "] =", total, "\n")
    
MPI.Finalize
