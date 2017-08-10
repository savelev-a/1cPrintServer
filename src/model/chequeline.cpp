#include "chequeline.h"

ChequeLine::ChequeLine()
{
    this->lineNumber = 0;
    this->artikul = "";
    this->barcode = "";
    this->name = "";
    this->quantity = 0;
    this->price = 0.0;
    this->discountPercent = 0.0;
    this->discount = 0.0;
    this->summ = 0.0;
}
