/**
 * Org:   Nankai University
 * Name:  Liu Zhuang
 * Email: 3331353075@qq.com
 * Date； 2020/12/11
 */

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>
/* To the the UUID (found the the TA's h-file(s)) */
#include <lenet5_ta.h>

#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/** Manually input.
#define FILE_TRAIN_IMAGE		"train-images-idx3-ubyte"
#define FILE_TRAIN_LABEL		"train-labels-idx1-ubyte"
#define FILE_TEST_IMAGE		"t10k-images-idx3-ubyte"
#define FILE_TEST_LABEL		"t10k-labels-idx1-ubyte"
*/
#define LENET_FILE 		"model.dat"
#define COUNT_TRAIN		60000
#define COUNT_TEST		10000

typedef unsigned char uint8;
typedef uint8 image[28][28];

int read_data(unsigned char(*data)[28][28], unsigned char label[], const int count, const char data_file[], const char label_file[])
{
    FILE *fp_image = fopen(data_file, "rb");
    FILE *fp_label = fopen(label_file, "rb");
    if (!fp_image||!fp_label) return 1;
	fseek(fp_image, 16, SEEK_SET);
	fseek(fp_label, 8, SEEK_SET);
	fread(data, sizeof(*data)*count, 1, fp_image);
	fread(label,count, 1, fp_label);
	fclose(fp_image);
	fclose(fp_label);
	return 0;
}

/**
 * The model now is initialed in TEE,
 * so instead of "LeNet5 *lenet",
 * we pass session pointer as the first param.
 * The following "testing" are the same.
 * */
//void training(LeNet5 *lenet, image *train_data, uint8 *train_label, int batch_size, int total_size)
void training(TEEC_Session* sess, image *train_data, uint8 *train_label, int batch_size, int total_size)
{
	for (int i = 0, percent = 0; i <= total_size - batch_size; i += batch_size)
    {
        TEEC_Operation op;
        uint32_t err_origin;
		TEEC_Result res;
        /*
	     * Prepare the argument. 
         * Pass temperary shared memory in the first and second parameter,
         * and we marked them as INPUT-only.
	     * the remaining two parameters are unused.
	     */
        
	    /* Clear the TEEC_Operation struct */
	    memset(&op, 0, sizeof(op));
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
                                         TEEC_NONE, TEEC_NONE);
        TEEC_TempMemoryReference tmr1, tmr2;
        tmr1.buffer = train_data + i;
        tmr1.size = batch_size * sizeof(image);
        tmr2.buffer = train_label + i;
        tmr2.size = batch_size * sizeof(uint8);
        op.params[0].tmpref = tmr1;
        op.params[1].tmpref = tmr2;

        //TrainBatch(lenet, train_data + i, train_label + i, batch_size);
        printf("Invoking TA to train lenet5 model with a batch.\n");
        res = TEEC_InvokeCommand(sess, TA_LENET5_CMD_TRAIN_BATCH, &op,
                                 &err_origin);
        if (res != TEEC_SUCCESS)
            errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
                 res, err_origin);

        if (i * 100 / total_size > percent)
            printf("batchsize:%d\ttrain:%2d%%\n", batch_size, percent = i * 100 / total_size);
    }
}

int testing(TEEC_Session* sess, image *test_data, uint8 *test_label,int total_size)
{
	int right = 0, percent = 0;
	for (int i = 0; i < total_size; ++i)
	{
        TEEC_Operation op;
        uint32_t err_origin;
		TEEC_Result res;
        /*
	     * Prepare the argument. 
         * Pass temperary shared memory in the first parameter(INPUT-only)
         * and a label number in second parameter(OUTPUT-only).
         * The result of prediction will be passed back via the second param.
	     * the remaining two parameters are unused.
	     */
        
	    /* Clear the TEEC_Operation struct */
	    memset(&op, 0, sizeof(op));
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_OUTPUT,
                                         TEEC_NONE, TEEC_NONE);
        TEEC_TempMemoryReference tmr;
        tmr.buffer = test_data + i;
        tmr.size = sizeof(image);
        op.params[0].tmpref = tmr;
		//int p = Predict(lenet, test_data[i], 10);
        printf("Invoking TA to make a prediction.\n");
        res = TEEC_InvokeCommand(sess, TA_LENET5_CMD_PREDICT, &op,
                                 &err_origin);
        if (res != TEEC_SUCCESS)
            errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
                 res, err_origin);
        uint32_t p = op.params[1].value.a;
        printf("Got a prediction number: %d\n", p);
		uint32_t l = test_label[i];
		right += l == p;
		if (i * 100 / total_size > percent)
			printf("test:%2d%%\n", percent = i * 100 / total_size);
	}
	return right;
}
/**
 * These teo functions are not used yet.
 * 
//Save the model
int save(LeNet5 *lenet, char filename[])
{
	FILE *fp = fopen(filename, "wb");
	if (!fp) return 1;
	fwrite(lenet, sizeof(LeNet5), 1, fp);
	fclose(fp);
	return 0;
}

//Read a model from file on the disk without training
int load(LeNet5 *lenet, char filename[])
{
	FILE *fp = fopen(filename, "rb");
	if (!fp) return 1;
	fread(lenet, sizeof(LeNet5), 1, fp);
	fclose(fp);
	return 0;
}
 */


void foo(TEEC_Session* sess)
{
	image *train_data = (image *)calloc(COUNT_TRAIN, sizeof(image));
	uint8 *train_label = (uint8 *)calloc(COUNT_TRAIN, sizeof(uint8));
	image *test_data = (image *)calloc(COUNT_TEST, sizeof(image));
	uint8 *test_label = (uint8 *)calloc(COUNT_TEST, sizeof(uint8));

	char FILE_TRAIN_IMAGE[128], FILE_TRAIN_LABEL[128];
	char FILE_TEST_IMAGE[128], FILE_TEST_LABEL[128];
	printf("Please input your train image data path:\n");
	scanf("%s", FILE_TRAIN_IMAGE);
	printf("Please input your train label path:\n");
	scanf("%s", FILE_TRAIN_LABEL);
	printf("Please input your test image data path:\n");
	scanf("%s", FILE_TEST_IMAGE);
	printf("Please input your test label path:\n");
	scanf("%s", FILE_TEST_LABEL);

	if (read_data(train_data, train_label, COUNT_TRAIN, FILE_TRAIN_IMAGE, FILE_TRAIN_LABEL))
	{
		printf("ERROR!!!\nDataset File Not Find!Please Copy Dataset to the Floder Included the exe\n");
		free(train_data);
		free(train_label);
		//system("pause");
		//pause();
	}

	if (read_data(test_data, test_label, COUNT_TEST, FILE_TEST_IMAGE, FILE_TEST_LABEL))
	{
		printf("ERROR!!!\nDataset File Not Find!Please Copy Dataset to the Floder Included the exe\n");
		free(test_data);
		free(test_label);
		//system("pause");
		//pause();
	}

	clock_t start = clock();

    /**
     * Replace it with a TEE invoke so that our model is stored in TEE
     */
	//LeNet5 *lenet = (LeNet5 *)malloc(sizeof(LeNet5));
    TEEC_Operation op;
    uint32_t err_origin;
	TEEC_Result res;
    
	/* Clear the TEEC_Operation struct */
	memset(&op, 0, sizeof(op));

    /*use no params to init*/
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
    printf("Invoking TA to initialize a lenet5 model.\n");
	res = TEEC_InvokeCommand(sess, TA_LENET5_CMD_INITIALIZE, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

	/**
     * Not used temporarily.
     * 
     * if (load(lenet, LENET_FILE))
	 *	    Initial(lenet);
     */
	int batches[] = { 300 };
	for (int i = 0; i < sizeof(batches) / sizeof(*batches);++i)
		training(sess, train_data, train_label, batches[i],COUNT_TRAIN);

	int right = testing(sess, test_data, test_label, COUNT_TEST);
	printf("%d/%d\n", right, COUNT_TEST);
	printf("Time:%u\n", (unsigned)(clock() - start));
	//save(lenet, LENET_FILE);

	free(train_data);
	free(train_label);
	free(test_data);
	free(test_label);
	//system("pause");
	//pause();
}

int main()
{
    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_UUID uuid = TA_LENET5_UUID;

	TEEC_Session sess;
	uint32_t err_origin;

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/*
	 * Open a session to the "hello world" TA, the TA will print "hello
	 * world!" in the log when the session is created.
	 */
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

    /*real function*/
	foo(&sess);

    TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return 0;
}

