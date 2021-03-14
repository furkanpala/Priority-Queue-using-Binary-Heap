/*
	Furkan Pala
	
	How to compile:
	g++ -std=c++11 main.cpp
	
	How to run:
	./a.out m p
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>

using namespace std;

class MinHeap
{
    vector<float> arr;

public:
    MinHeap(int, float);
    void MinHeapify(int);
    void Insert(float);
    void DecreaseKey(int, float);
    float ExtractMin();
    int Size();
};

MinHeap::MinHeap(int m, float p)
{
    // I am increasing vector capacity.
    // Otherwise, it would be initialized with a small capacity
    // and it is costly to copy elements when it's size reaches capacity

    // Expected value of taxis in heap is
    // operation amount times probability of adding a new taxi
    // m * (1 - p)
    arr.reserve((size_t)ceil(m * (1 - p)));
}

void MinHeap::MinHeapify(int index)
{
    int leftIndex = 2 * index + 1;
    int rightIndex = 2 * index + 2;

    int minIndex = index;

    if (leftIndex < arr.size() && arr[leftIndex] < arr[index])
        minIndex = leftIndex;

    if (rightIndex < arr.size() && arr[rightIndex] < arr[minIndex])
        minIndex = rightIndex;

    if (minIndex != index)
    {
        float temp = arr[index];
        arr[index] = arr[minIndex];
        arr[minIndex] = temp;

        MinHeapify(minIndex);
    }
}

void MinHeap::Insert(float element)
{
    arr.push_back(element);

    int i = arr.size() - 1;

    // Compare element with its parent
    // parent(i) = (i - 1) / 2
    while (i > 0 && arr[i] < arr[(i - 1) / 2])
    {
        float temp = arr[(i - 1) / 2];
        arr[(i - 1) / 2] = arr[i];
        arr[i] = temp;

        i = (i - 1) / 2;
    }
}

void MinHeap::DecreaseKey(int index, float extracted)
{
    // If distance is going to be negative
    // set it to zero
    if (arr[index] - extracted < 0)
        arr[index] = 0;
    else
        arr[index] -= extracted;

    // Compare element with its parent
    // parent(i) = (i - 1) / 2
    while (index > 0 && arr[index] < arr[(index - 1) / 2])
    {
        float temp = arr[(index - 1) / 2];
        arr[(index - 1) / 2] = arr[index];
        arr[index] = temp;

        index = (index - 1) / 2;
    }
}

float MinHeap::ExtractMin()
{
    if (arr.size() == 0)
    {
        cerr << "Heap underflow" << endl;
        exit(1);
    }

    // Get the root
    float minElement = arr[0];

    // Get the last element
    float lastElement = arr[arr.size() - 1];

    // Pop the last element
    arr.pop_back();

    if (arr.size() > 0)
    {
        // Overwrite the root with the last element
        arr[0] = lastElement;
        MinHeapify(0);
    }

    // Return the root
    return minElement;
}

int MinHeap::Size()
{
    return arr.size();
}

int main(int argc, char *argv[])
{
    //---RUNTIME MEASUREMENT START---
    auto start = chrono::high_resolution_clock::now();
    ifstream inFile("locations.txt");

    if (!inFile)
    {
        cerr << "File cannot be opened!" << endl;
        return 1;
    }

    if (argc < 3)
    {
        cerr << "Unsufficient command line arguments." << endl;
        return 1;
    }

    int m = atoi(argv[1]);
    float p = stof(argv[2]);

    string line;
    getline(inFile, line); // To read the header line

    MinHeap distances(m, p);

    const float hotelLongitude = 33.40819f;
    const float hotelLatitude = 39.19001f;

    vector<float> calledTaxiDistances;
    calledTaxiDistances.reserve(m / 100);

    int taxiAdditionCount = 0;
    int taxiDistanceUpdateCount = 0;

    srand(time(nullptr));

    for (int i = 0; i < m; i++)
    {
        // Every 100th, 200th, 300th... operation
        // call a taxi
        if (((i + 1) % 100) == 0)
            calledTaxiDistances.push_back(distances.ExtractMin());
        else
        {
            float randomNumber = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); // Random number in range [0,1]

            //---DISTANCE UPDATE PART---
            // If there are no elements in the heap
            // skip update and make taxi addition (enter the else below)
            if (distances.Size() > 0 && randomNumber <= p)
            {
                int randomIndex = rand() % distances.Size();

                distances.DecreaseKey(randomIndex, 0.01f);

                taxiDistanceUpdateCount++;
            }
            //---TAXI ADDITION PART---
            else
            {
                float longitude, latitude;
                inFile >> longitude;
                inFile >> latitude;

                getline(inFile, line, '\n');

                float distance = sqrtf((longitude - hotelLongitude) * (longitude - hotelLongitude) + (latitude - hotelLatitude) * (latitude - hotelLatitude));
                distances.Insert(distance);

                taxiAdditionCount++;
            }
        }
    }

    inFile.close();

    auto stop = chrono::high_resolution_clock::now();
    //---RUNTIME MEASUREMENT END---

    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    if (calledTaxiDistances.size() > 0)
    {
        for (auto &distance : calledTaxiDistances)
            cout << "A taxi has been called, distance: " << distance << endl;
    }
    else
        cout << "No taxi has been called" << endl;

    cout << "The number of taxi aditions: " << taxiAdditionCount << endl;
    cout << "The number of taxi distance updates: " << taxiDistanceUpdateCount << endl;
    cout << "Program execution time: " << duration.count() / 1000.f << " ms" << endl;

    return 0;
}