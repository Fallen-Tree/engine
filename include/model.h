#ifndef MODEL_H
#define MODEL_H

#include <vector>



class model {

    private:

        float* arrayOfPoints;
        unsigned int * arrayOfIndices;
        int lengthOfArrayOfPoints, lengthOfArrayOfIndices;
    
    public: 
    
        // some variables 
        unsigned int VAO, VBO, EBO;

        // constructor of class.
        model(std::vector<float>& arrayOfPoints, std::vector<unsigned int>& arrayOfIndices);
        model(std::vector<float>& arrayOfPoints);

        model();

        // deconstructor of class.
        ~model();

        // getter and setter for working with class.
        float* getArrayOfPoints();

        unsigned int* getArrayOfIndices();

        void setArrayOfPoints(std::vector<float>& arrayOfPoints);

        void setArrayOfIndices(std::vector<int>& arrayOfIndices);
        
        // get length of array.
        int getLengthOfArrayOfIndices();

        int getLengthofArrayOfPoints();
};
#endif