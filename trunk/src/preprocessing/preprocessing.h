#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include <stdio.h>
#include "../model/datatypes.h"

/*
// The list of subclass axioms that are generated during  preprocessing
SubClassAxiom** generated_subclass_axioms;
int generated_subclass_axiom_count;

// The list of subrole axioms that are generated during  preprocessing
SubRoleAxiom** generated_subrole_axioms = NULL;
int generated_subrole_axiom_count = 0;
*/

void preprocess_tbox(TBox* tbox);

void preprocess_abox(ABox* abox);

#endif
