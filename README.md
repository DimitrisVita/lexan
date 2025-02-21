# Lexan

## Description
This program analyzes text files and counts the frequency of words, excluding specific words from exclusion lists. It uses processes to split the text into segments and count the words in parallel, improving performance. The splitter processes divide the text into segments, while the builder processes count the frequency of words in each segment. The results are collected and combined to produce the final output, which is stored in an output file.

## Exclusion List
Words are read as they are, without any additional processing.

## Word Management
A word is considered a sequence of characters without spaces, as long as it does not contain any special characters or numbers.

## Sending Words from Splitters to Builders
Splitters read the text character by character and form words. When a word is read, they check if it is in the exclusion list. If not, they calculate the word length and create a message containing the word length and the word itself. This message is sent to the appropriate builder via a pipe. The builder is chosen based on a hash function that calculates an index from the sum of the characters of the word and the number of builders. Splitters continue this process until the entire text is read or the end of the designated text segment is reached.

## Sending Words from Builders to Root
Builders read words from stdin and update their occurrence count in a map. For each word in the map, they create a message containing the word length, the word, and its occurrence count. This message is sent via stdout to the root process. The root process reads these messages, stores them in an array, and writes them to the output file.

## Files and Functions

### lexan.c
- **argumentParsing**: Parses command-line arguments.
  - Complexity: O(n), where n is the number of arguments.
- **saveStartDescriptors**: Calculates and stores start points for each splitter.
  - Complexity: O(n), where n is the number of characters in the text file.
- **createPipes**: Creates pipes for inter-process communication.
  - Complexity: O(n), where n is the number of pipes.
- **createSplitterProcesses**: Creates splitter processes.
  - Complexity: O(n), where n is the number of splitter processes.
- **createBuilderProcesses**: Creates builder processes.
  - Complexity: O(n), where n is the number of builder processes.
- **readFromPipe**: Reads data from pipes.
  - Complexity: O(k), where k is the number of words or records read.
- **writeToFile**: Writes results to the output file.
  - Complexity: O(n), where n is the number of words in the list.

### splitter.c
- **readExclusionList**: Reads the exclusion list file and adds each line to the exclusion set.
  - Complexity: O(l), where l is the number of lines in the exclusion list file.
- **sendWordToBuilder**: Sends a word to the appropriate builder process via a pipe.
  - Complexity: O(1), as the word length is fixed and the sending is done to one of the numOfBuilders pipes.
- **processTextFile**: Reads the text file and sends each word to the appropriate builder, excluding words in the exclusion list.
  - Complexity: O(w), where w is the number of words in the text file.
  - Overall Complexity: O(w + l + n), where w is the number of words in the text file, l is the number of lines in the exclusion list, and n is the number of builder processes.

### builder.c
- **readWord**: Reads a word from stdin and stores it in the word variable.
  - Complexity: O(1), as it reads one word and only a few data.
- **updateWordCount**: Updates the word count in the Map data structure. If the word already exists, it increments its count; otherwise, it adds it with a count of 1.
  - Complexity: O(1) for searching and inserting or updating in the Map.
- **sendWordCount**: Sends the word and its count to stdout.
  - Complexity: O(1), as it simply sends the word and the count.
- **sendTime**: Sends the execution time to the root process.
  - Complexity: O(1), as it sends only one value.

## Compilation
To compile the project, use the provided Makefile. Run the following command in the project directory:
```sh
make
```

## Execution
To run the program, use the following command:
```sh
./lexan -i textFile -l numOfSplitter -m numOfBuilders -t topPopular -e exclusionList -o outputFile
```

## Test Files
Test files are available in the `test` folder. You can use these files to test the functionality of the program.