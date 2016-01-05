// Nome: errorTable.h
// Progetto: CPP/Base
// Autore: Ruffaldi Emanuele
// Error Table: each error is specified as
// enumerator, la tabella delle stringhe deve essere sincronizzata
// l'errore va espresso tutto a maiuscole
enum ErrorCode {
	FirstFatal,				
	FirstError,
		errEXPECTEDCONST,		// -
		errILLEGALVOID,			// -
		errCANNOTCONVERT,		// op, tipo1, tipo2
		errCANNOTCONVERT_PTR,	// op, tipo1, tipo2
		errLVALUEREQUIRED,		//
		errPTRREQUIRED,
		errVOIDRETURNING,
		errSUBSCRIPTREQUIRESARRAY,
		errSUBSCRIPTREQUIRESINTEGRAL,
		errCANNOTUSEVOIDPOINTER,
		errPOINTERADDSUBNOTIMPLEMENTED,
		errOPERATOROVERLOADINGNOTIMPLEMENTED,
		errINTEGRALREQUIRED, // op
		errSCALARREQUIRED,	// op
		errCANNOTDOTYPECAST,
		errCALLREQUIRESFUNCTIONOPERAND,
		errTOOMUCHARGUMENTS,
		errTOOLESSARGUMENTS,
		errARGUMENTTYPEMISMATCH, // indice, tipofrom , tipoto
		errFUNCTIONMUSTRETURNVALUE,	// nome
		errNOTALLCONTROLPATHSRETURN,	// nome
		errPTRADDITIONNOTALLOWED,
		errCANNOTSUBDIFFPTR,
		errFUNCCANNOTBEARRAY,
		errSYMBOLNOTFOUND,
		errFUNCPROTOREDEF,
		errSYMBOLREDEF,
		errFUNCREDEF,
		errPTRCASTERROR, 
		errCANNOTCASTTOARRAYORFUNCTION,
		// Parser Errors
		errREQUIREDTOKEN,	// token da 
		errREQUIREDDECL, 
		errBODYWITHOUTFUNC,
		errEXPSEMICOMMA,
		errSTATNOTIMPLEMENTED,
		errARGSAFTERELLIPSE, 
	FirstWarning,
	LastError
};
