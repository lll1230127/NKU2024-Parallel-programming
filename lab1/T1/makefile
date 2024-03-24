CC = gcc
CFLAGS = -Wall -O0
TARGET1 = T1
SRC1 = T1.c
TARGET2 = T1-2
SRC2 = T1-2.c

$(TARGET1): $(SRC1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(SRC1)

$(TARGET2): $(SRC2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SRC2)

test1: $(TARGET1)
	for /L %%n in (200,200,5000) do ( \
		$(TARGET1) %%n \
	)

test2: $(TARGET2)
	for /L %%n in (200,200,5000) do ( \
		$(TARGET2) %%n \
	)

clean:
	del /F /Q $(TARGET1)
	del /F /Q $(TARGET2)