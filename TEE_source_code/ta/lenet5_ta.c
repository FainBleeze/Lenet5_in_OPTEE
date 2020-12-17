/**
 * Org:   Nankai University
 * Name:  Liu Zhuang
 * Email: 3331353075@qq.com
 * Date； 2020/12/11
 */
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <lenet5_ta.h>
#include <lenet5.h>

/*
 * Called when the instance of the TA is created. This is the first call in the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("TA_CreateEntryPoint has been called");

	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
	DMSG("TA_DestroyEntryPoint has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("TA_OpenSessionEntryPoint has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	/*
	 * The DMSG() macro is non-standard, TEE Internal API doesn't
	 * specify any means to logging from a TA.
	 */
	IMSG("Lenet5 is created!\n");

	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	TEE_Free(lenet);/* Free the memory here.(In fact, it shall be automatically freed.)*/
	IMSG("Model freed, Goodbye!\n");
}

static TEE_Result ta_init(uint32_t param_types,
	TEE_Param params[4])
{
    /*No param allowed*/
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("Lenet5 initializing...");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	(void)&params; /* Unused parameter */

	//Use TEE internal API to mallocate memory for lenet5 model
    lenet = (LeNet5 *)TEE_Malloc(sizeof(LeNet5), TEE_MALLOC_FILL_ZERO);
	Initial();

	return TEE_SUCCESS;
}

static TEE_Result ta_trainBatch(uint32_t param_types,
	TEE_Param params[4])
{
	image* imageBuf;
	uint8* labelBuf;
	int num;
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
						   TEE_PARAM_TYPE_MEMREF_INPUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);
	DMSG("TEE model training...");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Read the parameters*/
	imageBuf = (image *)params[0].memref.buffer;
	labelBuf = (uint8 *)params[1].memref.buffer;
	num = params[0].memref.size / sizeof(image);

	/** API documentation mentioned that CA's memory should be read only once,
	 * but this model won't read the same memory twice or more.
	 * And my CA won't change any memory during the training.
	 * So I will skip this for now.
		 * Check if the TA is the only owner of the memory. 
		 * If not, we should make a copy to avoid CA's possibly changing memory.
		 * 
		if (TEE_SUCCESS != TEE_CheckMemoryAccessRights(TEE_MEMORY_ACCESS_READ, params[0].memref.buffer, params[0].memref.size)){
			imageBuf = TEE_Malloc(params[0].memref.size, TEE_MALLOC_FILL_ZERO);
			TEE_MemMove(imageBuf, params[0].memref.buffer, params[0].memref.size);
		}
		if (TEE_SUCCESS != TEE_CheckMemoryAccessRights(TEE_MEMORY_ACCESS_READ, params[1].memref.buffer, params[1].memref.size)){
			imageBuf = TEE_Malloc(params[1].memref.size, TEE_MALLOC_FILL_ZERO);
			TEE_MemMove(imageBuf, params[1].memref.buffer, params[1].memref.size);
		}
	*/

	//Train
    TrainBatch(imageBuf, labelBuf, num);

	return TEE_SUCCESS;
}

static TEE_Result ta_predict(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
						   TEE_PARAM_TYPE_VALUE_OUTPUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("Predicting in TEE...");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	// Predct.
    params[1].value.a = Predict(*(image *)params[0].memref.buffer);

	return TEE_SUCCESS;
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */
	switch (cmd_id) {
	case TA_LENET5_CMD_INITIALIZE:
		return ta_init(param_types, params);
	case TA_LENET5_CMD_TRAIN_BATCH:
		return ta_trainBatch(param_types, params);
	case TA_LENET5_CMD_PREDICT:
		return ta_predict(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
