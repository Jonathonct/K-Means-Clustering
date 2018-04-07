

The following implementation of the K-Means Clustering algorithm was written by Jonathon Teague and updated 4/3/2018
Please send any questions, comments or concerns to Jonathonct@gmail.com
This version of the K-Means Clustering algorithm was written and compiled on Microsoft's Visual Studio 2017 in C++. K-Means sorts numerical data into K-clusters, where K is a user-entered number.

Advantages of the K-Means algorithm:

    Relatively fast compared to other clustering algorithms
    Can attain above 90% accuracy on certain data sets
    Can identify previously unknown trends in data

    Disadvantages of the K-Means algorithm:
    Slow on learning sets with over 2,000 entries
    Cannot work with inconsistent data
    Does not work well with boolean/categorical data that has been converted to arbitrary numeric form
    Works best with a K-value between 2-4


Example data sets to work on, courtesy of UCI's machine learning website:
Iris data set: https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data
Wine data set: https://archive.ics.uci.edu/ml/machine-learning-databases/wine/
Breast cancer data set: https://archive.ics.uci.edu/ml/machine-learning-databases/breast-cancer-wisconsin/wdbc.data
*Updated to be able to read in missing data entries, marked as '?' and non-CSV formatted files
*I am currently working on resolving several other bugs relating to certain data sets and formatting
Use guide:

    Compile the 5 files ending in .h or .cpp together in Microsoft's Visual Studio 2017
    Run the program, enter the name of the file to use for learning and pick the value K for clustering.
        You can also choose to use 10% of the learning file for testing after creating the clusters
    Choose from the following menu options:
        'Display' to display which points fit into which clusters
            Type 'More' to see the mean value for each variable within each cluster
        'Spread' to show the spread of each identified variable (if given in learning file)
        'Predict' to make predictions on a file using the clusters created
        'Log' to save the results, including the latest predictions to a file
        'New' to run K-means on a new data set, or with new parameters
        'Quit' to exit the program

