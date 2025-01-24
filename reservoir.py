import numpy as np
from scipy.sparse import random
from scipy.sparse.linalg import eigs 

class Reservoir():

    def __init__(self, ):
        return
    
    def build_reservoir(self, N_res=100, eta=0.1, rho = 0.9):
        """
        Building the weight matrix of the reservoir
        
        Parameters: 
            N_res (int): number of neurons in the reservoir
            eta (float): the sparsity of the weight matrix
            rho (float): the spectral radius of the matrix
        """
        def uniform_rvs(size):
            return np.random.uniform(low=-1.0, high=1.0, size=size)
        
        # Generate a sparse matrix with uniform random values in [-1, 1]
        W = random(N_res, N_res, density=eta, format='csr', data_rvs=uniform_rvs)

        #Calculate the max eigenvalue and find the max
        max_eigenvalue = eigs(W, k=1, which='LM', return_eigenvectors=False)[0].real

        # Normalize W to get W_res
        rho = 0.9
        self.W_res = rho * (W / max_eigenvalue)

    def forward(self, input):
        
        return