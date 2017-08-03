# Linkage Map and Total Link Table Generator for EC Production Structures

This program takes a Forward Linkage map and generates a total forward link table and Critical Contributors table as demonstrated here:
http://onlinelibrary.wiley.com/doi/10.1111/0022-4146.00053/abstract
It creates two new .csv files with the specified tables.
It takes inputs of the Linkage map and the number of sectors per region. 
You must now specify the number of reported critical contributors, the threshold for general contributors,
and additionally, you must input the lower and upper bound of reported key sectors.


Recommended usage is running the .exe from command line with the form

LinkageBuilder.exe "inputfile.csv" [number of sectors per region] [number of reported critical contributors] [lower bound] [upper bound]


If you want to generate a a FL/BL comparison table using the general sectors the file must be run with no arguments.
You must specify all information in that case, as you are prompted.
