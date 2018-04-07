

The following implementation of the K-Means Clustering algorithm was written by Jonathon Teague and updated 4/3/2018
Please send any questions, comments or concerns to Jonathonct@gmail.com
K-Means sorts numerical data into K-clusters, where K is a user-entered number. To learn the basics of the algorithm:
https://en.wikipedia.org/wiki/K-means_clustering

Advantages of the K-Means algorithm: </br >
    - Relatively fast compared to other clustering algorithms </br >
    - Can attain above 90% accuracy on certain data sets </br >
    - Can identify previously unknown trends in data </br >

Disadvantages of the K-Means algorithm: </br >
    - Can take a long time to process a large learning set </br >
    - Requires consistent numerical data </br >
    - Does not work well with boolean/categorical data that has been converted to arbitrary numeric form </br >


Example data sets to work on, courtesy of UCI's machine learning website:
Iris data set: https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data
Wine data set: https://archive.ics.uci.edu/ml/machine-learning-databases/wine/
Breast cancer data set: https://archive.ics.uci.edu/ml/machine-learning-databases/breast-cancer-wisconsin/wdbc.data
*Updated to be able to read in missing data entries, marked as '?' and non-CSV formatted files
*I am currently working on resolving several other bugs relating to certain data sets and formatting
Use guide:

   1. Compile the 5 files ending in .h or .cpp together in Microsoft's Visual Studio 2017
   2. Run the program, enter the name of the file to use for learning and pick the value K for clustering.
        - You can also choose to use 10% of the learning file for testing after creating the clusters
   3. Choose from the following menu options:
        - 'Display' to display which points fit into which clusters
            - Type 'More' to see the mean value for each variable within each cluster
        - 'Spread' to show the spread of each identified variable (if given in learning file)
        - 'Predict' to make predictions on a file using the clusters created
        - 'Log' to save the results, including the latest predictions to a file
        - 'New' to run K-means on a new data set, or with new parameters
        - 'Quit' to exit the program
