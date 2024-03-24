CC = gcc
CFLAGS = -Wall -O0
TARGET1 = T2
SRC1 = T2.c
TARGET2 = T2-2
SRC2 = T2-2.c
TARGET3 = T2-3
SRC3 = T2-3.c


$(TARGET1): $(SRC1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(SRC1)

$(TARGET2): $(SRC2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SRC2)

$(TARGET3): $(SRC3)
	$(CC) $(CFLAGS) -o $(TARGET3) $(SRC3)

test1: $(TARGET1)
	for /L %%n in (200,200,10000) do ( \
		$(TARGET1) %%n \
	)

test2: $(TARGET2)
	for /L %%n in (200,200,10000) do ( \
		$(TARGET2) %%n \
	)

test3: $(TARGET3)
	for /L %%n in (200,200,10000) do ( \
		$(TARGET3) %%n \
	)

clean:
	del /F /Q $(TARGET1)
	del /F /Q $(TARGET2)