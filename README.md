Lab 6 contains a the code for a database server and webserver I build during my one of my earlier classes in the CS curriculumn. 

Part 1 contains a database lookup server called mdb-lookup-server. It uses sockets API to establish a connection with a client and, after receiving a client request, it forwards the request to a data base built in a previous lab. The mdb server then returns the content from the data base.

Part 2 contains an http-server which downloads a webpage's HTML content and writes it to a local html file. The server establishes a connection with the host using sockets, sends a GET request (HTTP 1.0). It receives the content, skipping the header and writing only the HTML content to the file.

Much of the socket code was skeleton code received from the professor.

If you have further questions please email me at JBullock252@gmail.com
