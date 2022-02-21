/**
  @file flare.c

  @brief Subroutines that control the output of XUV flares.

  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

  @date Mar 25 2016

*/

#include "vplanet.h"
#include <math.h>
#include <stdio.h>

void BodyCopyFlare(BODY *dest, BODY *src, int foo, int iNumBodies, int iBody) {
  dest[iBody].dFlareMinEnergy  = src[iBody].dFlareMinEnergy;
  dest[iBody].dFlareMaxEnergy  = src[iBody].dFlareMaxEnergy;
  dest[iBody].dLXUVFlareConst  = src[iBody].dLXUVFlareConst;
  dest[iBody].dLXUVFlare       = src[iBody].dLXUVFlare;
  dest[iBody].dLXUVFlareUpper  = src[iBody].dLXUVFlareUpper;
  dest[iBody].dLXUVFlareLower  = src[iBody].dLXUVFlareLower;
  dest[iBody].iFlareFFD        = src[iBody].iFlareFFD;
  dest[iBody].iFlareBandPass   = src[iBody].iFlareBandPass;
  dest[iBody].iFlareSlopeUnits = src[iBody].iFlareSlopeUnits;
  dest[iBody].dEnergyBin       = src[iBody].dEnergyBin;
  dest[iBody].dFlareSlope      = src[iBody].dFlareSlope;
  dest[iBody].dFlareYInt       = src[iBody].dFlareYInt;
}

/**************** FLARE options ********************/

void ReadFlareEnergyBin(BODY *body,
                        CONTROL *control,
                        FILES *files,
                        OPTIONS *options,
                        SYSTEM *system,
                        int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,
                  options->cName,
                  &dTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dEnergyBin = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].dEnergyBin = options->dDefault;
}

void ReadFlareFFD(BODY *body,
                  CONTROL *control,
                  FILES *files,
                  OPTIONS *options,
                  SYSTEM *system,
                  int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,
                  options->cName,
                  cTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "da", 2)) {
      body[iFile - 1].iFlareFFD = FLARE_FFD_DAVENPORT;
    } else if (!memcmp(sLower(cTmp), "la", 2)) {
      body[iFile - 1].iFlareFFD = FLARE_FFD_LACY;
    } else if (!memcmp(sLower(cTmp), "no", 2)) {
      body[iFile - 1].iFlareFFD = FLARE_FFD_NONE;
    } else {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are DAVENPORT, "
                "LACY, or NONE.\n",
                options->cName,
                cTmp);
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].iFlareFFD = FLARE_FFD_DAVENPORT;
}

void ReadFlareSlopeUnits(BODY *body,
                         CONTROL *control,
                         FILES *files,
                         OPTIONS *options,
                         SYSTEM *system,
                         int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,
                  options->cName,
                  cTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "se", 2)) {
      body[iFile - 1].iFlareSlopeUnits = FLARE_SLOPE_SEC;
    } else if (!memcmp(sLower(cTmp), "mi", 2)) {
      body[iFile - 1].iFlareSlopeUnits = FLARE_SLOPE_MINUTE;
    } else if (!memcmp(sLower(cTmp), "ho", 2)) {
      body[iFile - 1].iFlareSlopeUnits = FLARE_SLOPE_HOUR;
    } else if (!memcmp(sLower(cTmp), "da", 2)) {
      body[iFile - 1].iFlareSlopeUnits = FLARE_SLOPE_DAY;
    } else {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are SEC, MIN, HOUR "
                "or DAY.\n",
                options->cName,
                cTmp);
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].iFlareSlopeUnits = FLARE_SLOPE_DAY;
}

void ReadFlareBandPass(BODY *body,
                       CONTROL *control,
                       FILES *files,
                       OPTIONS *options,
                       SYSTEM *system,
                       int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,
                  options->cName,
                  cTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "ke", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_KEPLER;
    } else if (!memcmp(sLower(cTmp), "uv", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_UV;
    } else if (!memcmp(sLower(cTmp), "go", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_GOES;
    } else if (!memcmp(sLower(cTmp), "sr", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_SXR;
    } else if (!memcmp(sLower(cTmp), "te", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_TESS_UV;
    } else if (!memcmp(sLower(cTmp), "bo", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_BOLOMETRIC;
    } else {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are KEPLER, UV, "
                "GOES, TESSUV, BOLOMETRIC and SXR.\n",
                options->cName,
                cTmp);
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].iFlareBandPass = FLARE_KEPLER;
}


/*void ReadFlareMinEnergy(BODY *body,CONTROL *control,FILES *files,OPTIONS
*options,SYSTEM *system,int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,
control->Io.iVerbose); if (dTmp < 0) body[iFile - 1].dFlareMinEnergy = dTmp *
dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose); else
      body[iFile - 1].dFlareMinEnergy = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].dFlareMinEnergy = options->dDefault;
}*/


/*void ReadFlareMinEnergy(BODY *body, CONTROL *control, FILES *files, OPTIONS
*options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareMinEnergy =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareMinEnergy = dTmp * fdUnitsEnergy(units->iTime,
units->iMass, units->iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareMinEnergy = options->dDefault;
  }
}
*/


void ReadFlareMinEnergy(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp > 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareMinEnergy =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareMinEnergy = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dFlareMinEnergy = options->dDefault;
    }
  }
}


void ReadFlareMaxEnergy(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareMaxEnergy =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareMaxEnergy = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dFlareMaxEnergy = options->dDefault;
    }
  }
}
/*
void ReadFlareMaxEnergy(BODY *body,
                        CONTROL *control,
                        FILES *files,
                        OPTIONS *options,
                        SYSTEM *system,
                        int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,
                  options->cName,
                  &dTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile - 1].dFlareMaxEnergy =
            dTmp * dNegativeDouble(*options,
                                   files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    else
      body[iFile - 1].dFlareMaxEnergy = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].dFlareMaxEnergy = options->dDefault;
}*/

void ReadFlareYInt(BODY *body,
                   CONTROL *control,
                   FILES *files,
                   OPTIONS *options,
                   SYSTEM *system,
                   int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,
                  options->cName,
                  &dTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dFlareYInt = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].dFlareYInt = options->dDefault;
}

// TODO: Include the error in the FFD slopes to calculate the upper and higher
// limit of XUV luminosity by flares
/*void ReadFlareYIntErrorUpper(BODY *body,
                           CONTROL *control,
                           FILES *files,
                           OPTIONS *options,
                           SYSTEM *system,
                           int iFile) {
// This parameter cannot exist in primary file
int lTmp = -1;
double dTmp;

AddOptionDouble(files->Infile[iFile].cIn,
                options->cName,
                &dTmp,
                &lTmp,
                control->Io.iVerbose);
if (lTmp >= 0) {
  NotPrimaryInput(iFile,
                  options->cName,
                  files->Infile[iFile].cIn,
                  lTmp,
                  control->Io.iVerbose);
  body[iFile - 1].dFlareYIntErrorUpper = dTmp;
  UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
} else if (iFile > 0)
  body[iFile - 1].dFlareYIntErrorUpper = options->dDefault;
}

void ReadFlareYIntErrorLower(BODY *body,
                           CONTROL *control,
                           FILES *files,
                           OPTIONS *options,
                           SYSTEM *system,
                           int iFile) {
// This parameter cannot exist in primary file
int lTmp = -1;
double dTmp;

AddOptionDouble(files->Infile[iFile].cIn,
                options->cName,
                &dTmp,
                &lTmp,
                control->Io.iVerbose);
if (lTmp >= 0) {
  NotPrimaryInput(iFile,
                  options->cName,
                  files->Infile[iFile].cIn,
                  lTmp,
                  control->Io.iVerbose);
  body[iFile - 1].dFlareYIntErrorLower = dTmp;
  UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
} else if (iFile > 0)
  body[iFile - 1].dFlareYIntErrorLower = options->dDefault;
}*/

void ReadFlareSlope(BODY *body,
                    CONTROL *control,
                    FILES *files,
                    OPTIONS *options,
                    SYSTEM *system,
                    int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,
                  options->cName,
                  &dTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dFlareSlope = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].dFlareSlope = options->dDefault;
}

// TODO: Include the error in the FFD slopes to calculate the upper and higher
// limit of XUV luminosity by flares
/*void ReadFlareSlopeErrorUpper(BODY *body,
                            CONTROL *control,
                            FILES *files,
                            OPTIONS *options,
                            SYSTEM *system,
                            int iFile) {
// This parameter cannot exist in primary file
int lTmp = -1;
double dTmp;

AddOptionDouble(files->Infile[iFile].cIn,
                options->cName,
                &dTmp,
                &lTmp,
                control->Io.iVerbose);
if (lTmp >= 0) {
  NotPrimaryInput(iFile,
                  options->cName,
                  files->Infile[iFile].cIn,
                  lTmp,
                  control->Io.iVerbose);
  body[iFile - 1].dFlareSlopeErrorUpper = dTmp;
  UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
} else if (iFile > 0)
  body[iFile - 1].dFlareSlopeErrorUpper = options->dDefault;
}

void ReadFlareSlopeErrorLower(BODY *body,
                            CONTROL *control,
                            FILES *files,
                            OPTIONS *options,
                            SYSTEM *system,
                            int iFile) {
// This parameter cannot exist in primary file
int lTmp = -1;
double dTmp;

AddOptionDouble(files->Infile[iFile].cIn,
                options->cName,
                &dTmp,
                &lTmp,
                control->Io.iVerbose);
if (lTmp >= 0) {
  NotPrimaryInput(iFile,
                  options->cName,
                  files->Infile[iFile].cIn,
                  lTmp,
                  control->Io.iVerbose);
  body[iFile - 1].dFlareSlopeErrorLower = dTmp;
  UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
} else if (iFile > 0)
  body[iFile - 1].dFlareSlopeErrorLower = options->dDefault;
}*/
void ReadLXUVFlareConst(BODY *body,
                        CONTROL *control,
                        FILES *files,
                        OPTIONS *options,
                        SYSTEM *system,
                        int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,
                  options->cName,
                  &dTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile - 1].dLXUVFlareConst =
            dTmp * dNegativeDouble(*options,
                                   files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    else
      body[iFile - 1].dLXUVFlareConst = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].dLXUVFlareConst = options->dDefault;
}

/* Initiatlize Input Options */

void InitializeOptionsFlare(OPTIONS *options, fnReadOption fnRead[]) {
  int iOpt, iFile;

  sprintf(options[OPT_FLAREYINT].cName, "dFlareYInt");
  sprintf(options[OPT_FLAREYINT].cDescr,
          "Y-Intercept for Flare Frequency"); // Where the curve intercepts the
                                              // y axis (y=ax+b, in this case,
                                              // the parameter is "b")
  sprintf(options[OPT_FLAREYINT].cDefault, "20.9 (Proxima)");
  options[OPT_FLAREYINT].dDefault   = 20.9;
  options[OPT_FLAREYINT].iType      = 2;
  options[OPT_FLAREYINT].bMultiFile = 1;
  fnRead[OPT_FLAREYINT]             = &ReadFlareYInt;

  // TODO: Include the error in the FFD slopes to calculate the upper and higher
  // limit of XUV luminosity by flares
  /*sprintf(options[OPT_FLAREYINTERRORUPPER].cName, "dFlareYIntErrorUpper");
  sprintf(options[OPT_FLAREYINTERRORUPPER].cDescr, "Y-Intercept upper error");
  sprintf(options[OPT_FLAREYINTERRORUPPER].cDefault, "0.0");
  options[OPT_FLAREYINTERRORUPPER].dDefault   = 0.0;
  options[OPT_FLAREYINTERRORUPPER].iType      = 2;
  options[OPT_FLAREYINTERRORUPPER].bMultiFile = 1;
  fnRead[OPT_FLAREYINTERRORUPPER]             = &ReadFlareYIntErrorUpper;

  sprintf(options[OPT_FLAREYINTERRORLOWER].cName, "dFlareYIntErrorLower");
  sprintf(options[OPT_FLAREYINTERRORLOWER].cDescr, "Y-Intercept lower error");
  sprintf(options[OPT_FLAREYINTERRORLOWER].cDefault, "0.0");
  options[OPT_FLAREYINTERRORLOWER].dDefault   = 0.0;
  options[OPT_FLAREYINTERRORLOWER].iType      = 2;
  options[OPT_FLAREYINTERRORLOWER].bMultiFile = 1;
  fnRead[OPT_FLAREYINTERRORLOWER]             = &ReadFlareYIntErrorLower;
*/
  sprintf(options[OPT_FLARESLOPE].cName, "dFlareSlope");
  sprintf(options[OPT_FLARESLOPE].cDescr, "Slope for Flare Frequency");
  sprintf(options[OPT_FLARESLOPE].cDefault, "-0.68 (Proxima)");
  options[OPT_FLARESLOPE].dDefault   = -0.68;
  options[OPT_FLARESLOPE].iType      = 2;
  options[OPT_FLARESLOPE].bMultiFile = 1;
  fnRead[OPT_FLARESLOPE]             = &ReadFlareSlope;

  // TODO: Include the error in the FFD slopes to calculate the upper and higher
  // limit of XUV luminosity by flares
  /*sprintf(options[OPT_FLARESLOPEERRORUPPER].cName, "dFlareSlopeErrorUpper");
  sprintf(options[OPT_FLARESLOPEERRORUPPER].cDescr, "Slope upper error");
  sprintf(options[OPT_FLARESLOPEERRORUPPER].cDefault, "0.0");
  options[OPT_FLARESLOPEERRORUPPER].dDefault   = 0.0;
  options[OPT_FLARESLOPEERRORUPPER].iType      = 2;
  options[OPT_FLARESLOPEERRORUPPER].bMultiFile = 1;
  fnRead[OPT_FLARESLOPEERRORUPPER]             = &ReadFlareSlopeErrorUpper;

  sprintf(options[OPT_FLARESLOPEERRORLOWER].cName, "dFlareSlopeErrorLower");
  sprintf(options[OPT_FLARESLOPEERRORLOWER].cDescr, "Slope lower error");
  sprintf(options[OPT_FLARESLOPEERRORLOWER].cDefault, "0.0");
  options[OPT_FLARESLOPEERRORLOWER].dDefault   = 0.0;
  options[OPT_FLARESLOPEERRORLOWER].iType      = 2;
  options[OPT_FLARESLOPEERRORLOWER].bMultiFile = 1;
  fnRead[OPT_FLARESLOPEERRORLOWER]             = &ReadFlareSlopeErrorLower;
*/


  sprintf(options[OPT_FLAREMINENERGY].cName, "dFlareMinEnergy");
  sprintf(options[OPT_FLAREMINENERGY].cDescr,
          "Minimum Flare Energy to consider");
  sprintf(options[OPT_FLAREMINENERGY].cDefault, "10^26 J");
  options[OPT_FLAREMINENERGY].dDefault   = 1e26;
  options[OPT_FLAREMINENERGY].iType      = 2;
  options[OPT_FLAREMINENERGY].bMultiFile = 1;
  options[OPT_FLAREMINENERGY].dNeg       = 1e-7;
  sprintf(options[OPT_FLAREMINENERGY].cNeg, "ergs");
  fnRead[OPT_FLAREMINENERGY] = &ReadFlareMinEnergy;

  sprintf(options[OPT_FLAREMAXENERGY].cName, "dFlareMaxEnergy");
  sprintf(options[OPT_FLAREMAXENERGY].cDescr,
          "Maximum Flare Energy to consider");
  sprintf(options[OPT_FLAREMAXENERGY].cDefault, "10^29 J");
  options[OPT_FLAREMAXENERGY].dDefault   = 1e29;
  options[OPT_FLAREMAXENERGY].iType      = 2;
  options[OPT_FLAREMAXENERGY].bMultiFile = 1;
  options[OPT_FLAREMAXENERGY].dNeg       = 1e-7;
  sprintf(options[OPT_FLAREMAXENERGY].cNeg, "ergs");
  fnRead[OPT_FLAREMAXENERGY] = &ReadFlareMaxEnergy;

  sprintf(options[OPT_LXUVFLARECONST].cName, "dLXUVFlareConst");
  sprintf(options[OPT_LXUVFLARECONST].cDescr, "XUV luminosity of flares");
  sprintf(options[OPT_LXUVFLARECONST].cDefault, "10^22 Watts or 10^29 erg/s");
  options[OPT_LXUVFLARECONST].dDefault   = 1e22;
  options[OPT_LXUVFLARECONST].iType      = 2;
  options[OPT_LXUVFLARECONST].bMultiFile = 1;
  options[OPT_LXUVFLARECONST].dNeg       = LSUN;
  sprintf(options[OPT_LXUVFLARECONST].cNeg, "LSUN");
  sprintf(options[OPT_LXUVFLARECONST].cDimension, "energy/time");
  fnRead[OPT_LXUVFLARECONST] = &ReadLXUVFlareConst;

  sprintf(options[OPT_FLAREENERGYBIN].cName, "dEnergyBin");
  sprintf(options[OPT_FLAREENERGYBIN].cDescr,
          "Number of energies consider between the minimum and maximum "
          "energies to calculate the luminosity by flares");
  sprintf(options[OPT_FLAREENERGYBIN].cDefault,
          "100 energies between dFlareMinEnergy and dFlareMaxEnergy");
  options[OPT_FLAREENERGYBIN].dDefault   = 100.0;
  options[OPT_FLAREENERGYBIN].iType      = 2;
  options[OPT_FLAREENERGYBIN].bMultiFile = 1;
  options[OPT_FLAREENERGYBIN].dNeg       = 1;
  sprintf(options[OPT_FLAREENERGYBIN].cNeg, "None");
  fnRead[OPT_FLAREENERGYBIN] = &ReadFlareEnergyBin;

  sprintf(options[OPT_FLAREFFD].cName, "sFlareFFD");
  sprintf(options[OPT_FLAREFFD].cDescr, "Modes of calculate the FFD");
  sprintf(options[OPT_FLAREFFD].cDefault, "DAVENPORT");
  sprintf(options[OPT_FLAREFFD].cValues, "DAVENPORT LACY NONE");
  options[OPT_FLAREFFD].iType      = 3;
  options[OPT_FLAREFFD].bMultiFile = 1;
  fnRead[OPT_FLAREFFD]             = &ReadFlareFFD;
  sprintf(options[OPT_FLAREFFD].cLongDescr,
          " If DAVENPORT is selected, the code will employ the model from "
          "Davenport et al.\n"
          "(2019ApJ...871..241D) the user have to give the mass and Stellar "
          "age, dMass \n"
          "and dAge, as well the maximum and minimum energy, dFlareMinEnergy "
          "and \n"
          "dFlareMaxEnergy, to calculate the linear and angular coeficients, "
          "dA and dB of\n"
          "the flare frequency distribution (FFD), and then the FFD in this "
          "energy range.\n"
          "If Lacy is selected, the code will employ the model of Lacy et al. "
          "(1976ApJS...30...85L),\n"
          "and the user have to give the same input of the DAVENPORT's mode, "
          "plus the \n"
          "linear and angular coeficients, dFlareSlope and dFlareYInt of the "
          "FFD. If NONE is selected,\n"
          "the code will use the XUV luminosity given by the user in the input "
          "file (dLXUVFlareConst). \n");

  sprintf(options[OPT_FLAREBANDPASS].cName, "sFlareBandPass");
  sprintf(options[OPT_FLAREBANDPASS].cDescr,
          "Options of band pass of the input energy of flares");
  sprintf(options[OPT_FLAREBANDPASS].cDefault, "KEPLER");
  sprintf(options[OPT_FLAREBANDPASS].cValues,
          "KEPLER UV GOES SXR BOLOMETRIC TESSUV");
  options[OPT_FLAREBANDPASS].iType      = 3;
  options[OPT_FLAREBANDPASS].bMultiFile = 1;
  fnRead[OPT_FLAREBANDPASS]             = &ReadFlareBandPass;
  sprintf(options[OPT_FLAREBANDPASS].cLongDescr,
          " If UV or GOES is selected, the code will convert the input energy "
          "of flares\n"
          "from the UV band (3000–4300 Å) or GOES band (1-8 Å) to kepler band "
          "(4000–9000 Å) to calculate.\n"
          " the FFD and to the SXR band (1.24 - 1239.85 Å) to calculate the "
          "luminosity.\n"
          " If SXR is selected, the code will convert the input energy of "
          "flares from the SXR band to kepler band \n"
          "to calculate the FFD and will use the same value of energy to "
          "calculate the luminosity .\n"
          " If KEPLER is selected, the code will use the same input energy to "
          "calculate the FFD\n"
          "and will convert the input energy to SXR band to calculate the "
          "luminosity.\n"
          "The convertion values are taken from Osten and Wolk (2015) "
          "(doi:10.1088/0004-637X/809/1/79).\n"
          "If the TESSUV its selected, the code will convert the input energy "
          "of flares\n"
          "using the conversion value to the band U (2000-2800 Å) to the TESS "
          "data, from \n"
          " Gunther et al 2020 (https://doi.org/10.3847/1538-3881/ab5d3a). \n"
          "If the BOLOMETRIC its selected, the code will convert the input "
          "energy of flares\n"
          "using the convertion values are taken from Osten and Wolk (2015) "
          "(doi:10.1088/0004-637X/809/1/79).\n");

  sprintf(options[OPT_FLARESLOPEUNITS].cName, "sFlareSlopeUnits");
  sprintf(options[OPT_FLARESLOPEUNITS].cDescr,
          "Options to the units of the FFD in which the FFD slopes are define");
  sprintf(options[OPT_FLARESLOPEUNITS].cDefault, "DAY");
  sprintf(options[OPT_FLARESLOPEUNITS].cValues, "SEC MIN HOUR DAY");
  options[OPT_FLARESLOPEUNITS].iType      = 3;
  options[OPT_FLARESLOPEUNITS].bMultiFile = 1;
  fnRead[OPT_FLARESLOPEUNITS]             = &ReadFlareSlopeUnits;
  sprintf(options[OPT_FLARESLOPEUNITS].cLongDescr,
          " This option allow the user input the units for the inputed FFD "
          "slopes. The options are\n"
          " SEC, for input slopes with FFD in number of flares per seconds, "
          "MIN for number of flares per minutes, \n"
          " HOUR for number of flares per hour, and DAY number of flares per "
          "for days.\n");
}

void ReadOptionsFlare(BODY *body,
                      CONTROL *control,
                      FILES *files,
                      OPTIONS *options,
                      SYSTEM *system,
                      fnReadOption fnRead[],
                      int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTFLARE; iOpt < OPTENDFLARE; iOpt++) {
    if (options[iOpt].iType != -1)
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
  }
}

/******************* Verify FLARE ******************/

void PropsAuxFlare(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                   int iBody) {
  /* Nothing */
}


void VerifyFlareFFD(BODY *body,
                    CONTROL *control,
                    OPTIONS *options,
                    UPDATE *update,
                    double dAge,
                    int iBody) {

  // Assign luminosity
  if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT) {
    body[iBody].dLXUVFlare =
          fdLXUVFlare(body, control->Evolve.dTimeStep, iBody);
    if (options[OPT_FLAREFFD].iLine[iBody + 1] >= 0) {
      if (control->Io.iVerbose >= VERBINPUT)
        printf("INFO: The FFD will follow Davenport et. al, 2019 model.\n");
    }
  } else if (body[iBody].iFlareFFD == FLARE_FFD_LACY) {
    body[iBody].dLXUVFlare =
          fdLXUVFlare(body, control->Evolve.dTimeStep, iBody);
    if (options[OPT_FLAREFFD].iLine[iBody + 1] >= 0) {
      if (control->Io.iVerbose >= VERBINPUT)
        printf("INFO: The FFD will remain constant during all the "
               "simulation.\n");
    }
  } else if (body[iBody].iFlareFFD == FLARE_FFD_NONE) {
    body[iBody].dLXUVFlare = body[iBody].dLXUVFlareConst;
    if (options[OPT_FLAREFFD].iLine[iBody + 1] >= 0) {
      if (control->Io.iVerbose >= VERBINPUT)
        printf("INFO: The FFD will remain constant during all the "
               "simulation.\n");
    }
  }
}

void VerifyLXUVFlare(BODY *body, OPTIONS *options, UPDATE *update, int iBody) {

  /* This may become useful once flare evolution is included
  update[iBody].iaType[update[iBody].iLXUV][0] = 1;
  update[iBody].iNumBodies[update[iBody].iLXUV][0]=1;
  update[iBody].iaBody[update[iBody].iLXUV][0] =
  malloc(update[iBody].iNumBodies[update[iBody].iLXUV][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iLXUV][0][0]=iBody;
  update[iBody].pdDLXUVFlareDt =
  &update[iBody].daDerivProc[update[iBody].iLXUV][0];*/
}


/*
void AssignStellarDerivatives(BODY *body,EVOLVE *evolve,UPDATE
*update,fnUpdateVariable ***fnUpdate,int iBody) {

}

void NullStellarDerivatives(BODY *body,EVOLVE *evolve,UPDATE
*update,fnUpdateVariable ***fnUpdate,int iBody) {
 fnUpdate[iBody][update[iBody].iLXUVFlare][0]                               =
&fndUpdateFunctionTiny; // NOTE: This points to the value of the Temperature!
}
*/

void AssignFlareDerivatives(BODY *body,
                            EVOLVE *evolve,
                            UPDATE *update,
                            fnUpdateVariable ***fnUpdate,
                            int iBody) {
  /* No derivatives yet for flare.
  //This may become useful once flare evolution is included*/
  // fnUpdate[iBody][update[iBody].iLXUV][0] = &fdLXUVFlare;
}

void NullFlareDerivatives(BODY *body,
                          EVOLVE *evolve,
                          UPDATE *update,
                          fnUpdateVariable ***fnUpdate,
                          int iBody) {
  /* No derivatives yet for flare.
  This may become useful once flare evolution is included*/
  fnUpdate[iBody][update[iBody].iLXUV][0] = &fndUpdateFunctionTiny;
}

void VerifyFlare(BODY *body,
                 CONTROL *control,
                 FILES *files,
                 OPTIONS *options,
                 OUTPUT *output,
                 SYSTEM *system,
                 UPDATE *update,
                 int iBody,
                 int iModule) {
  int iFile = iBody + 1;
  /* Mass must be in proper range */

  if (body[iBody].dMass < MINMASSFLARE * MSUN ||
      body[iBody].dMass > MAXMASSFLARE * MSUN) {
    fprintf(stderr,
            "ERROR: Mass of %s must be between %.3lf and %.3lf\n",
            body[iBody].cName,
            MINMASSFLARE,
            MAXMASSFLARE);
    LineExit(files->Infile[iBody + 1].cIn, options[OPT_MASS].iLine[iBody + 1]);
  }
  /* If the XUV luminosity by flares is given by the user, the code do not allow
  output FFD and flare energies because the code do not calculate: the user
  already inputed the XUV luminosity by flares and the flare module doesn't need
  this information anymore*/
  if (body[iBody].iFlareFFD == FLARE_FFD_NONE) {
    int iCol, bError = 0;
    for (iCol = 0; iCol < files->Outfile[iBody].iNumCols; iCol++) {
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQ1].cName,
                 strlen(output[OUT_FLAREFREQ1].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQ1].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQ2].cName,
                 strlen(output[OUT_FLAREFREQ2].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQ2].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQ3].cName,
                 strlen(output[OUT_FLAREFREQ3].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQ3].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQ4].cName,
                 strlen(output[OUT_FLAREFREQ4].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQ4].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQ5].cName,
                 strlen(output[OUT_FLAREFREQ5].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQ5].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQ6].cName,
                 strlen(output[OUT_FLAREFREQ6].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQ6].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQMAX].cName,
                 strlen(output[OUT_FLAREFREQMAX].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQMAX].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGY1].cName,
                 strlen(output[OUT_FLAREENERGY1].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGY1].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGY2].cName,
                 strlen(output[OUT_FLAREENERGY2].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGY2].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGY3].cName,
                 strlen(output[OUT_FLAREENERGY3].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGY3].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGY4].cName,
                 strlen(output[OUT_FLAREENERGY4].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGY4].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGY5].cName,
                 strlen(output[OUT_FLAREENERGY5].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGY5].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGY6].cName,
                 strlen(output[OUT_FLAREENERGY6].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGY6].cName);
        bError = 1;
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGYMAX].cName,
                 strlen(output[OUT_FLAREENERGYMAX].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGYMAX].cName);
        bError = 1;
      }
    }
  }

  /* if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT) || (body[iBody].iFlareFFD
   == FLARE_FFD_LACY) { if (body[iBody].dFlareMinEnergy = 0) { fprintf(stderr,
   "ERROR: %s must be >= 0. ", input[OPT_FLAREMINENERGY].cName);
       }
   }*/


  VerifyLXUVFlare(body, options, update, iBody);
  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorFlare;
  control->fnPropsAux[iBody][iModule]        = &PropsAuxFlare;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyFlare;
}


void fnForceBehaviorFlare(BODY *body,
                          MODULE *module,
                          EVOLVE *evolve,
                          IO *io,
                          SYSTEM *system,
                          UPDATE *update,
                          fnUpdateVariable ***fnUpdate,
                          int iBody,
                          int iModule) {

  if (body[iBody].dLXUVFlare < 0)
    body[iBody].dLXUVFlare = 0;
  else
    body[iBody].dLXUVFlare = fdLXUVFlare(body, evolve->dTimeStep, iBody);
}


void InitializeBodyFlare(BODY *body, CONTROL *control, UPDATE *update,
                         int iBody, int iModule) {
  body[iBody].dLXUVFlare = fdLXUVFlare(body, control->Evolve.dTimeStep, iBody);
}

void InitializeModuleFlare(CONTROL *control, MODULE *module) {
  /* Anything Here? */
}

/**************** FLARE update ****************/

void InitializeUpdateFlare(BODY *body, UPDATE *update, int iBody) {
  /* STELLAR calculates LXUV from the star's properties, but FLARE calculates
   * LXUV as a primary variable. It is the only possible update. */

  /* No primary variables for FLARE yet
  update[iBody].iNumVars++;
  update[iBody].iNumLXUV++;
  */
}

void FinalizeUpdateLXUVFlare(BODY *body, UPDATE *update, int *iEqn, int iVar,
                             int iBody, int iFoo) {
  /* No primary variables for FLARE yet*/
  update[iBody].iaModule[iVar][*iEqn] = FLARE;
  update[iBody].iNumLXUV              = (*iEqn)++;
}

// TODO: Maybe add this to each FlareFreq output? Energies do not evolve, so do
// not need this.
/*void FinalizeUpdateFlareFreqMax(BODY *body, UPDATE *update, int *iEqn, int
  iVar, int iBody, int iFoo) {
  /* No primary variables for FLARE yet*/
/*  update[iBody].iaModule[iVar][*iEqn] = FLARE;
  update[iBody].iNumFlareFreqMax              = (*iEqn)++;
}
*/

/***************** FLARE Halts *****************/

/* Halt for massive flare? No Flares? */

void CountHaltsFlare(HALT *halt, int *iNumHalts) {
}

void VerifyHaltFlare(BODY *body, CONTROL *control, OPTIONS *options, int iBody,
                     int *iHalt) {
}

/************* FLARE Outputs ******************/

/* NOTE: If you write a new Write subroutine here you need to add the associate
   block of initialization in InitializeOutputFlare below */

void WriteLXUVFlare(BODY *body,
                    CONTROL *control,
                    OUTPUT *output,
                    SYSTEM *system,
                    UNITS *units,
                    UPDATE *update,
                    int iBody,
                    double *dTmp,
                    char cUnit[]) {
  *dTmp = fdLXUVFlare(body, control->Evolve.dTimeStep, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

// TODO: Include the error in the FFD slopes to calculate the upper and higher
// limit of XUV luminosity by flares
/*
void WriteLXUVFlareUpper(BODY *body,
                         CONTROL *control,
                         OUTPUT *output,
                         SYSTEM *system,
                         UNITS *units,
                         UPDATE *update,
                         int iBody,
                         double *dTmp,
                         char cUnit[]) {
  *dTmp = body[iBody].dLXUVFlareUpper;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}


void WriteLXUVFlareLower(BODY *body,
                         CONTROL *control,
                         OUTPUT *output,
                         SYSTEM *system,
                         UNITS *units,
                         UPDATE *update,
                         int iBody,
                         double *dTmp,
                         char cUnit[]) {
  *dTmp = body[iBody].dLXUVFlareLower;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}
*/
void WriteFlareFreq1(BODY *body,
                     CONTROL *control,
                     OUTPUT *output,
                     SYSTEM *system,
                     UNITS *units,
                     UPDATE *update,
                     int iBody,
                     double *dTmp,
                     char cUnit[]) {
  *dTmp = body[iBody].dFlareFreq1;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreq2(BODY *body,
                     CONTROL *control,
                     OUTPUT *output,
                     SYSTEM *system,
                     UNITS *units,
                     UPDATE *update,
                     int iBody,
                     double *dTmp,
                     char cUnit[]) {
  *dTmp = body[iBody].dFlareFreq2;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreq3(BODY *body,
                     CONTROL *control,
                     OUTPUT *output,
                     SYSTEM *system,
                     UNITS *units,
                     UPDATE *update,
                     int iBody,
                     double *dTmp,
                     char cUnit[]) {
  *dTmp = body[iBody].dFlareFreq3;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreq4(BODY *body,
                     CONTROL *control,
                     OUTPUT *output,
                     SYSTEM *system,
                     UNITS *units,
                     UPDATE *update,
                     int iBody,
                     double *dTmp,
                     char cUnit[]) {
  *dTmp = body[iBody].dFlareFreq4;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreqMin(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char cUnit[]) {
  *dTmp = body[iBody].dFlareFreqMin;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreqMid(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char cUnit[]) {
  *dTmp = body[iBody].dFlareFreqMid;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreqMax(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char cUnit[]) {
  *dTmp = body[iBody].dFlareFreqMax;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareEnergy1(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergy1;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}

void WriteFlareEnergy2(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergy2;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}

void WriteFlareEnergy3(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergy3;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}

void WriteFlareEnergy4(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergy4;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}
void WriteFlareEnergyMin(BODY *body,
                         CONTROL *control,
                         OUTPUT *output,
                         SYSTEM *system,
                         UNITS *units,
                         UPDATE *update,
                         int iBody,
                         double *dTmp,
                         char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergyMin;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}
void WriteFlareEnergyMid(BODY *body,
                         CONTROL *control,
                         OUTPUT *output,
                         SYSTEM *system,
                         UNITS *units,
                         UPDATE *update,
                         int iBody,
                         double *dTmp,
                         char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergyMid;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}

void WriteFlareEnergyMax(BODY *body,
                         CONTROL *control,
                         OUTPUT *output,
                         SYSTEM *system,
                         UNITS *units,
                         UPDATE *update,
                         int iBody,
                         double *dTmp,
                         char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergyMax;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}
void InitializeOutputFlare(OUTPUT *output, fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_FLAREFREQ1].cName, "FlareFreq1");
  sprintf(output[OUT_FLAREFREQ1].cDescr,
          "Frequency of the flares with the lowest energy");
  sprintf(output[OUT_FLAREFREQ1].cNeg, "/day");
  output[OUT_FLAREFREQ1].bNeg       = 1;
  output[OUT_FLAREFREQ1].dNeg       = 1;
  output[OUT_FLAREFREQ1].iNum       = 1;
  output[OUT_FLAREFREQ1].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ1]           = &WriteFlareFreq1;

  sprintf(output[OUT_FLAREFREQ2].cName, "FlareFreq2");
  sprintf(output[OUT_FLAREFREQ2].cDescr, "Flare frequency");
  sprintf(output[OUT_FLAREFREQ2].cNeg, "/day");
  output[OUT_FLAREFREQ2].bNeg       = 1;
  output[OUT_FLAREFREQ2].dNeg       = 1;
  output[OUT_FLAREFREQ2].iNum       = 1;
  output[OUT_FLAREFREQ2].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ2]           = &WriteFlareFreq2;

  sprintf(output[OUT_FLAREFREQ3].cName, "FlareFreq3");
  sprintf(output[OUT_FLAREFREQ3].cDescr, "Flare frequency");
  sprintf(output[OUT_FLAREFREQ3].cNeg, "/day");
  output[OUT_FLAREFREQ3].bNeg       = 1;
  output[OUT_FLAREFREQ3].dNeg       = 1;
  output[OUT_FLAREFREQ3].iNum       = 1;
  output[OUT_FLAREFREQ3].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ3]           = &WriteFlareFreq3;

  sprintf(output[OUT_FLAREFREQ4].cName, "FlareFreq4");
  sprintf(output[OUT_FLAREFREQ4].cDescr, "Flare frequency");
  sprintf(output[OUT_FLAREFREQ4].cNeg, "/day");
  output[OUT_FLAREFREQ4].bNeg       = 1;
  output[OUT_FLAREFREQ4].dNeg       = 1;
  output[OUT_FLAREFREQ4].iNum       = 1;
  output[OUT_FLAREFREQ4].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ4]           = &WriteFlareFreq4;

  sprintf(output[OUT_FLAREFREQMIN].cName, "FlareFreqMin");
  sprintf(output[OUT_FLAREFREQMIN].cDescr,
          "Frequency of the flares with the lowest energy");
  sprintf(output[OUT_FLAREFREQMIN].cNeg, "/day");
  output[OUT_FLAREFREQMIN].bNeg       = 1;
  output[OUT_FLAREFREQMIN].dNeg       = 1;
  output[OUT_FLAREFREQMIN].iNum       = 1;
  output[OUT_FLAREFREQMIN].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQMIN]           = &WriteFlareFreqMin;

  sprintf(output[OUT_FLAREFREQMID].cName, "FlareFreqMid");
  sprintf(output[OUT_FLAREFREQMID].cDescr,
          "Frequency of the flares with the middle energy in the energy range");
  sprintf(output[OUT_FLAREFREQMIN].cNeg, "/day");
  output[OUT_FLAREFREQMIN].bNeg       = 1;
  output[OUT_FLAREFREQMIN].dNeg       = 1;
  output[OUT_FLAREFREQMIN].iNum       = 1;
  output[OUT_FLAREFREQMIN].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQMIN]           = &WriteFlareFreqMid;

  sprintf(output[OUT_FLAREFREQMAX].cName, "FlareFreqMax");
  sprintf(output[OUT_FLAREFREQMAX].cDescr,
          "Frequency of the flares with the highest energy");
  sprintf(output[OUT_FLAREFREQMAX].cNeg, "/day");
  output[OUT_FLAREFREQMAX].bNeg       = 1;
  output[OUT_FLAREFREQMAX].dNeg       = 1;
  output[OUT_FLAREFREQMAX].iNum       = 1;
  output[OUT_FLAREFREQMAX].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQMAX]           = &WriteFlareFreqMax;


  sprintf(output[OUT_FLAREENERGY1].cName, "FlareEnergy1");
  sprintf(output[OUT_FLAREENERGY1].cDescr, "Minimum flare energy");
  sprintf(output[OUT_FLAREENERGY1].cNeg, "ergs");
  output[OUT_FLAREENERGY1].bNeg       = 1;
  output[OUT_FLAREENERGY1].dNeg       = 1.0e7;
  output[OUT_FLAREENERGY1].iNum       = 1;
  output[OUT_FLAREENERGY1].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY1]           = &WriteFlareEnergy1;

  sprintf(output[OUT_FLAREENERGY2].cName, "FlareEnergy2");
  sprintf(output[OUT_FLAREENERGY2].cDescr, "Flare energy");
  sprintf(output[OUT_FLAREENERGY2].cNeg, "ergs");
  output[OUT_FLAREENERGY2].bNeg       = 1;
  output[OUT_FLAREENERGY2].dNeg       = 1.0e7;
  output[OUT_FLAREENERGY2].iNum       = 1;
  output[OUT_FLAREENERGY2].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY2]           = &WriteFlareEnergy2;

  sprintf(output[OUT_FLAREENERGY3].cName, "FlareEnergy3");
  sprintf(output[OUT_FLAREENERGY3].cDescr, "Flare energy");
  sprintf(output[OUT_FLAREENERGY3].cNeg, "ergs");
  output[OUT_FLAREENERGY3].bNeg       = 1;
  output[OUT_FLAREENERGY3].dNeg       = 1.0e7;
  output[OUT_FLAREENERGY3].iNum       = 1;
  output[OUT_FLAREENERGY3].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY3]           = &WriteFlareEnergy3;

  sprintf(output[OUT_FLAREENERGY4].cName, "FlareEnergy4");
  sprintf(output[OUT_FLAREENERGY4].cDescr, "Flare energy");
  sprintf(output[OUT_FLAREENERGY4].cNeg, "ergs");
  output[OUT_FLAREENERGY4].bNeg       = 1;
  output[OUT_FLAREENERGY4].dNeg       = 1.0e7;
  output[OUT_FLAREENERGY4].iNum       = 1;
  output[OUT_FLAREENERGY4].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY4]           = &WriteFlareEnergy4;

  sprintf(output[OUT_FLAREENERGYMIN].cName, "FlareEnergyMin");
  sprintf(output[OUT_FLAREENERGYMIN].cDescr, "Minimum flare energy");
  sprintf(output[OUT_FLAREENERGYMIN].cNeg, "ergs");
  output[OUT_FLAREENERGYMIN].bNeg       = 1;
  output[OUT_FLAREENERGYMIN].dNeg       = 1.0e7;
  output[OUT_FLAREENERGYMIN].iNum       = 1;
  output[OUT_FLAREENERGYMIN].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGYMIN]           = &WriteFlareEnergyMin;

  sprintf(output[OUT_FLAREENERGYMID].cName, "FlareEnergyMid");
  sprintf(output[OUT_FLAREENERGYMID].cDescr,
          "Middle flare energy in the range of energy values");
  sprintf(output[OUT_FLAREENERGYMID].cNeg, "ergs");
  output[OUT_FLAREENERGYMID].bNeg       = 1;
  output[OUT_FLAREENERGYMID].dNeg       = 1.0e7;
  output[OUT_FLAREENERGYMID].iNum       = 1;
  output[OUT_FLAREENERGYMID].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGYMID]           = &WriteFlareEnergyMid;

  sprintf(output[OUT_FLAREENERGYMAX].cName, "FlareEnergyMax");
  sprintf(output[OUT_FLAREENERGYMAX].cDescr, "Maximum flare energy");
  sprintf(output[OUT_FLAREENERGYMAX].cNeg, "ergs");
  output[OUT_FLAREENERGYMAX].bNeg       = 1;
  output[OUT_FLAREENERGYMAX].dNeg       = 1.0e7;
  output[OUT_FLAREENERGYMAX].iNum       = 1;
  output[OUT_FLAREENERGYMAX].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGYMAX]           = &WriteFlareEnergyMax;


  sprintf(output[OUT_LXUVFLARE].cName, "LXUVFlare");
  sprintf(output[OUT_LXUVFLARE].cDescr, "XUV Luminosity from flares");
  sprintf(output[OUT_LXUVFLARE].cNeg, "LSUN");
  output[OUT_LXUVFLARE].bNeg       = 1;
  output[OUT_LXUVFLARE].dNeg       = 1. / LSUN;
  output[OUT_LXUVFLARE].iNum       = 1;
  output[OUT_LXUVFLARE].iModuleBit = FLARE;
  fnWrite[OUT_LXUVFLARE]           = &WriteLXUVFlare;

  // TODO: Include the error in the FFD slopes to calculate the upper and higher
  // limit of XUV luminosity by flares
  /*sprintf(output[OUT_LXUVFLAREUPPER].cName, "LXUVFlareUpper");
  sprintf(output[OUT_LXUVFLAREUPPER].cDescr,
          "Upper limit value to XUV Luminosity from flares when considerer "
          "flare rate slope and Y-intercept errors");
  sprintf(output[OUT_LXUVFLAREUPPER].cNeg, "LSUN");
  output[OUT_LXUVFLAREUPPER].bNeg       = 1;
  output[OUT_LXUVFLAREUPPER].dNeg       = 1. / LSUN;
  output[OUT_LXUVFLAREUPPER].iNum       = 1;
  output[OUT_LXUVFLAREUPPER].iModuleBit = FLARE;
  fnWrite[OUT_LXUVFLAREUPPER]           = &WriteLXUVFlareUpper;

  sprintf(output[OUT_LXUVFLARELOWER].cName, "LXUVFlareLower");
  sprintf(output[OUT_LXUVFLARELOWER].cDescr,
          "Lower limit value to XUV Luminosity from flares when considerer "
          "flare rate slope and Y-intercept errors");
  sprintf(output[OUT_LXUVFLARELOWER].cNeg, "LSUN");
  output[OUT_LXUVFLARELOWER].bNeg       = 1;
  output[OUT_LXUVFLARELOWER].dNeg       = 1. / LSUN;
  output[OUT_LXUVFLARELOWER].iNum       = 1;
  output[OUT_LXUVFLARELOWER].iModuleBit = FLARE;
  fnWrite[OUT_LXUVFLARELOWER]           = &WriteLXUVFlareLower;*/
}

/************ FLARE Logging Functions **************/

void LogOptionsFlare(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- FLARE Options -----\n\n");
  */
}

void LogFlare(BODY *body,
              CONTROL *control,
              OUTPUT *output,
              SYSTEM *system,
              UPDATE *update,
              fnWriteOutput fnWrite[],
              FILE *fp) {

  /* Anything here?
   int iOut;

   fprintf(fp,"\n----- FLARE PARAMETERS ------\n");
   for (iOut=OUTSTARTFLARE;iOut<OUTBODYSTARTFLARE;iOut++) {
     if (output[iOut].iNum > 0)
       WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
   }
   */
}

void LogBodyFlare(BODY *body,
                  CONTROL *control,
                  OUTPUT *output,
                  SYSTEM *system,
                  UPDATE *update,
                  fnWriteOutput fnWrite[],
                  FILE *fp,
                  int iBody) {
  int iOut;

  fprintf(fp, "----- FLARE PARAMETERS (%s)------\n", body[iBody].cName);
  for (iOut = OUTSTARTFLARE; iOut < OUTENDFLARE; iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,
                    control,
                    &output[iOut],
                    system,
                    update,
                    fnWrite[iOut],
                    fp,
                    iBody);
  }
}

void AddModuleFlare(CONTROL *control, MODULE *module, int iBody, int iModule) {
  module->iaModule[iBody][iModule] = FLARE;

  module->fnCountHalts[iBody][iModule]        = &CountHaltsFlare;
  module->fnReadOptions[iBody][iModule]       = &ReadOptionsFlare;
  module->fnLogBody[iBody][iModule]           = &LogBodyFlare;
  module->fnVerify[iBody][iModule]            = &VerifyFlare;
  module->fnAssignDerivatives[iBody][iModule] = &AssignFlareDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullFlareDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltFlare;

  module->fnInitializeUpdate[iBody][iModule]   = &InitializeUpdateFlare;
  module->fnInitializeBody[iBody][iModule]     = &InitializeBodyFlare;
  module->fnFinalizeUpdateLXUV[iBody][iModule] = &FinalizeUpdateLXUVFlare;

  /* XXX I think this approach to multi-module outputs is defunct
  module->fnFinalizeOutputFunction[iBody][iModule] =
  &FinalizeOutputFunctionFlare;
  */
}

/************* FLARE Functions ************/

// Functions to convert the input band pass to the right one (SXR)

double fdBandPassXUV(BODY *body, int iBody, double dInputEnergy) {
  double dLogEnergyXUV = 0.0;

  if (body[iBody].iFlareBandPass == FLARE_KEPLER) { // Band pass 4000 – 9000 Å
    // 1.875 comes from the Osten & Wolk 2015
    // (https://ui.adsabs.harvard.edu/abs/2015ApJ...809...79O/abstract), this is
    // in Table 2 - the SXR makes up 30% of the bolometric energy of the flare,
    // and the Kepler bandpass makes up 16%. So if you have the Kepler energy or
    // flux, multiply it by 30/16=1.875 to get the SXR energy.
    dLogEnergyXUV = log10(dInputEnergy * 1.875);
  } else if (body[iBody].iFlareBandPass ==
             FLARE_UV) { // Band pass 3000 – 4300 Å
    dLogEnergyXUV = log10(dInputEnergy * 2.727);
  } else if (body[iBody].iFlareBandPass == FLARE_GOES) { // Band pass 1 - 8 Å
    dLogEnergyXUV = log10(dInputEnergy * 5);
  } else if (body[iBody].iFlareBandPass ==
             FLARE_SXR) { // Band pass 1.24 - 1239.85 Å
    dLogEnergyXUV = log10(dInputEnergy);
  }
  /* TODO: Leave it here for when the data in the future allows for the
    conversion of a kepler bandpass to these another bandpass. else if
    (body[iBody].iFlareBandPass == FLARE_FUV) { // Band pass 1.24 - 1239.85 Å
     dEnergyXUV = dInputEnergy;
     dLogEnergyXUV = log10(dInputEnergy);
    }
    else if (body[iBody].iFlareBandPass == FLARE_NUV) { // Band pass 1.24 -
    1239.85 Å dEnergyXUV = dInputEnergy; dLogEnergyXUV = log10(dInputEnergy);
    }
    else if (body[iBody].iFlareBandPass == FLARE_LYALPHA) { // Band pass 1.24 -
    1239.85 Å dEnergyXUV = dInputEnergy; dLogEnergyXUV = log10(dInputEnergy);
    } */
  else if (body[iBody].iFlareBandPass ==
           FLARE_TESS_UV) { // Band pass 1.24 - 1239.85 Å
    dLogEnergyXUV = log10(dInputEnergy * (0.3 / 0.076));
  } else if (body[iBody].iFlareBandPass ==
             FLARE_BOLOMETRIC) { // Band pass 1.24 - 1239.85 Å
    dLogEnergyXUV = log10(dInputEnergy * 0.3);
  }
  return dLogEnergyXUV;
}


double fdBandPassKepler(BODY *body, int iBody, double dInputEnergy) {
  double dLogEnergy = 0.0;

  if (body[iBody].iFlareBandPass == FLARE_KEPLER) { // Band pass 4000 – 9000 Å
    dLogEnergy = log10(dInputEnergy);
  } else if (body[iBody].iFlareBandPass ==
             FLARE_UV) { // Band pass 3000 – 4300 Å
    dLogEnergy = log10(dInputEnergy * 1.455);
  } else if (body[iBody].iFlareBandPass == FLARE_GOES) { // Band pass 1 - 8 Å
    dLogEnergy = log10(dInputEnergy * 2.667);
  } else if (body[iBody].iFlareBandPass ==
             FLARE_SXR) { // Band pass 1.24 - 1239.85 Å
    dLogEnergy = log10(dInputEnergy * 0.5334);
  } else if (body[iBody].iFlareBandPass == FLARE_TESS_UV) { // Band pass 1 - 8 Å
    dLogEnergy = log10(dInputEnergy * 2.10526315789);
  } else if (body[iBody].iFlareBandPass ==
             FLARE_BOLOMETRIC) { // Band pass 1.24 - 1239.85 Å
    dLogEnergy = log10(dInputEnergy * 0.16);
  }
  return dLogEnergy;
}


// Davenport parameters of a star with mass dStarMass and age dStarAge
double fdDavenport(double dA1, double dA2, double dA3, double dStarAge,
                   double dStarMass) {
  // Davenport function recives log(ergs) and return log(flare/day)
  // Calculating the dA and dB parameters with the Lacy et. al 1976
  // (1976ApJS...30...85L) equation
  double dA = 0.0;
  dStarAge  = dStarAge / (YEARSEC * 1.0e6);
  // Converting dStarAge from seconds to million years
  // because Davenport et al. 2019 model only accepts Myr.
  dStarMass = dStarMass /
              MSUN; // Converting dStarMass from kg to solar mass because the
                    // Davenport et al. 2019 model only accepts Msun. The
                    // StarMass is divided by a factor of 1.99e30 (solar mass)
                    // because when the user define it in vpl.in they give it in
                    // solar masses, but the code converts it back to kg.
  dA = (dA1 * log10(dStarAge)) + dA2 * (dStarMass) + dA3;

  return dA;
}
// fdFFD calculates the convertion between the units that are given by the user
// (through dFlareYInt AND dFlareSlope) and the units that vplanet understand,
// i.e., SI units.
double fdFFD(BODY *body,
             int iBody,
             double dLogEnergy,
             double dFlareSlope,
             double dFlareYInt) {
  double dFlareFreq, dFFDAY;
  double dFFD = 0.0;
  dFlareFreq  = (dFlareSlope * dLogEnergy) +
               (dFlareYInt); // Here the Flare frequency are in log(flares/day).

  if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT) {
    dFFD = pow(10, dFlareFreq); // Here the Flare frequency are in flares/day.
    dFFD = dFFD / DAYSEC; // Here the Flare frequency are in flares/second.
  }
  if (body[iBody].iFlareFFD == FLARE_FFD_LACY) {
    if (body[iBody].iFlareSlopeUnits == FLARE_SLOPE_DAY) {
      dFFD = pow(10, dFlareFreq); // Here the Flare frequency are in flares/day.
      dFFD = dFFD / DAYSEC; // Here the Flare frequency are in flares/second.
    } else if (body[iBody].iFlareSlopeUnits == FLARE_SLOPE_SEC) {
      dFFD = pow(10,
                 dFlareFreq); // Here the Flare frequency are in flares/second.
    } else if (body[iBody].iFlareSlopeUnits == FLARE_SLOPE_HOUR) {
      dFFD = pow(10, dFlareFreq); // Here the Flare frequency are in
                                  // flares/hour.
      dFFD = dFFD / 3600; // Here the Flare frequency are in flares/second.
    } else if (body[iBody].iFlareSlopeUnits == FLARE_SLOPE_MINUTE) {
      dFFD = pow(10,
                 dFlareFreq); // Here the Flare frequency are in flares/minute.
      dFFD = dFFD / 60;       // Here the Flare frequency are in flares/second.
    } else {
      dFFD = pow(10, dFlareFreq); // Here the Flare frequency are in flares/day.
      dFFD = dFFD / DAYSEC; // Here the Flare frequency are in flares/second.
    }
  }
  return dFFD;
}

double fdEnergyJoulesXUV(double dLogEnergyXUV) {
  double dEnergyJOUXUV;

  dEnergyJOUXUV = pow(10, (dLogEnergyXUV));

  return dEnergyJOUXUV;
}


double fdLXUVFlare(BODY *body, double dDeltaTime, int iBody) {
  double dFlareSlope = 0.0;
  double dFlareYInt  = 0.0;
  double dStarAge, dStarMass;
  double dLXUVFlare = 0.0;
  double dLogEnergyMinERG, dLogEnergyMaxERG, dEnergyMin, dEnergyMax,
        dLogEnergyMin, dLogEnergyMax;
  int i, iEnergyBin, iLogEnergyMinERG, iLogEnergyMaxERG, iLogEnergyMin,
        iLogEnergyMax;
  double dEnergyMinXUV, dEnergyMaxXUV, dLogEnergyMinXUV;
  int iLogEnergyMinERGXUV, iLogEnergyMaxERGXUV, iLogEnergyMinXUV,
        iLogEnergyMaxXUV;
  double dEnergyStep, dEnergyStepXUV, dEnergyBin;


  //######################### 1. Choosing how to calculate FFD: slopes(age) or
  // slopes(constant)?##################################

  if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT) {
    // The coefficient values given here were given by Dr. James Davenport in
    // private comunication
    dFlareSlope = fdDavenport(-0.07054598,
                              0.81225239,
                              -1.07054511,
                              body[iBody].dAge,
                              body[iBody].dMass);
    dFlareYInt  = fdDavenport(2.06012734,
                              -25.79885288,
                              34.44115635,
                              body[iBody].dAge,
                              body[iBody].dMass);
  } else if (body[iBody].iFlareFFD == FLARE_FFD_LACY) {
    dFlareSlope = body[iBody].dFlareSlope;
    dFlareYInt  = body[iBody].dFlareYInt;
    // TODO: Let this here for allow the user input the slopes errors in the
    // future
    /*dFlareSlopeErrorUpper = body[iBody].dFlareSlopeErrorUpper;
    dFlareSlopeErrorLower = body[iBody].dFlareSlopeErrorLower;
    dFlareYIntErrorUpper = body[iBody].dFlareYIntErrorUpper;
    dFlareYIntErrorLower = body[iBody].dFlareYIntErrorLower;*/
  }
  if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT ||
      body[iBody].iFlareFFD == FLARE_FFD_LACY) {
    //################# 2. Calculating the XUV energy (SXR 1.24 - 1239.85
    //Å)#######################################################

    dLogEnergyMinXUV = fdBandPassXUV(body, iBody, body[iBody].dFlareMinEnergy);

    // Defining the array size (dEnergybin) of energies
    dEnergyBin = body[iBody].dEnergyBin;
    iEnergyBin = (int)dEnergyBin;

    // Declaring the XUV Energy arrays of size dEnergyBin
    double daEnergyERGXUV[iEnergyBin + 1], daEnergyJOUXUV[iEnergyBin + 1],
          daLogEnerXUV[iEnergyBin + 1];

    //################# 3. Calculating the energy in the Kepler band pass (4000
    //– 9000 Å) ##############################################

    dLogEnergyMin = fdBandPassKepler(body, iBody, body[iBody].dFlareMinEnergy);
    dLogEnergyMax = fdBandPassKepler(body, iBody, body[iBody].dFlareMaxEnergy);

    // 1.0 J = 1.0e7 ergs, but this is in log, so we have to sum 7.0, not
    // multiply by 1.0e7 Convert the units of the energy from Joules to ergs
    // because Davenport et al. 2019 model only accepts energy in ergs.

    dLogEnergyMinERG = dLogEnergyMin + 7.0;
    dLogEnergyMaxERG = dLogEnergyMax + 7.0;

    // Defining the energy step used in line 1097, 1099, and 1101 to fill the
    // energy arrays
    dEnergyStep = (dLogEnergyMaxERG - dLogEnergyMinERG) / iEnergyBin;

    // Declaring the Kepler Energy arrays of size iEnergyBin
    double daEnergyERG[iEnergyBin + 1], daEnergyJOU[iEnergyBin + 1],
          daLogEner[iEnergyBin + 1], daEnerJOU[iEnergyBin + 1];

    //############################ 4. Filling the energy arrays
    //########################################################################

    for (i = 0; i < iEnergyBin + 1; i++) {
      // XUV energy (energy_joules)
      daEnergyJOUXUV[i] = fdEnergyJoulesXUV(dLogEnergyMinXUV + i * dEnergyStep);
      // Kepler energy (log(energy_ergs))
      daLogEner[i] = dLogEnergyMinERG + i * dEnergyStep;
      // Kepler energy (energy_joules)
      daEnerJOU[i] = pow(10, (dLogEnergyMin + i * dEnergyStep));
    }

    body[iBody].dFlareEnergy1   = daEnerJOU[0];
    body[iBody].dFlareEnergy2   = daEnerJOU[1];
    body[iBody].dFlareEnergy3   = daEnerJOU[2];
    body[iBody].dFlareEnergy4   = daEnerJOU[3];
    body[iBody].dFlareEnergy5   = daEnerJOU[4];
    body[iBody].dFlareEnergy6   = daEnerJOU[5];
    body[iBody].dFlareEnergyMin = daEnerJOU[0];
    body[iBody].dFlareEnergyMid = daEnerJOU[iEnergyBin / 2];
    body[iBody].dFlareEnergyMax = daEnerJOU[iEnergyBin];
    //############################ 5. Filling the FFD arrays
    //########################################################################

    // Declaring the Flare Frequency distribution (FFD) arrays of size
    // dEnergyBin
    double daFFD[iEnergyBin + 1];

    // When DAVENPORT or LACY are selected, we have to calculate the FFD first.
    for (i = 0; i < iEnergyBin + 1; i++) {
      daFFD[i] = fdFFD(body, iBody, daLogEner[i], dFlareSlope, dFlareYInt);
    }

    body[iBody].dFlareFreq1   = daFFD[0];
    body[iBody].dFlareFreq2   = daFFD[1];
    body[iBody].dFlareFreq3   = daFFD[2];
    body[iBody].dFlareFreq4   = daFFD[3];
    body[iBody].dFlareFreq5   = daFFD[4];
    body[iBody].dFlareFreq6   = daFFD[5];
    body[iBody].dFlareFreqMin = daFFD[0];
    body[iBody].dFlareFreqMid = daFFD[iEnergyBin / 2];
    body[iBody].dFlareFreqMax = daFFD[iEnergyBin];
    //############################ 6. Calculating the XUV luminosity by flares
    //########################################################################
    double daLXUVFlare[iEnergyBin];

    // Calculating the luminosity by flares for DAVENPORT or LACY mode
    // if the user select to calculate the luminosity using a FFD model
    for (i = 0; i < iEnergyBin; i++) {
      daLXUVFlare[i] = (daEnergyJOUXUV[i + 1] - daEnergyJOUXUV[i]) *
                       ((daFFD[i + 1] + daFFD[i]) / 2);
      dLXUVFlare += daLXUVFlare[i];
    }
  }
  // If the FFD model is set to NONE, the luminosity remains constant over the
  // time evolution of the system and receives the value given by the user in
  // the input file
  else if (body[iBody].iFlareFFD == FLARE_FFD_NONE) {
    dLXUVFlare = 0.0;
    dLXUVFlare = body[iBody].dLXUVFlareConst;
  }
  /*  else {
          for(i = 0; i < iEnergyBin; i++) {
            daLXUVFlare[i] =
    (daEnergyJOUXUV[i+1]-daEnergyJOUXUV[i])*((daFFD[i+1] + daFFD[i])/2);
            dLXUVFlare += daLXUVFlare[i];
          }
    }*/

  return dLXUVFlare;
}
