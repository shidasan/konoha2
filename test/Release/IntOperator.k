/* this test is written by shinpei nakata */

void global_setup() { 
	
}

global_setup();
boolean int_operator_add() { 
	int i1 = 1;
	int i2 = 2;
	System.p( 1  +  2 );
	System.p( 1  +  i1 );
	System.p( i1  +  1 );
	System.p( i1  +  i2 );
}
boolean int_operator_sub() { 
	int i6 = 6;
	int i3 = 3;
	System.p( 5  -  4 );
	System.p( 5  -  i6 );
	System.p( i6  -  5 );
	System.p( i6  -  i3 );
}
boolean int_operator_mul() { 
	int i6 = 6;
	int i3 = 3;
	System.p( 5  *  4 );
	System.p( 5  *  i6 );
	System.p( i6  *  5 );
	System.p( i6  *  i3 );
}
boolean int_operator_div() { 
	int i6 = 6;
	int i3 = 3;
	System.p( 5  /  4 );
	System.p( 5  /  i6 );
	System.p( i6  /  5 );
	System.p( i6  /  i3 );
}
boolean int_operator_mod() { 
	int i6 = 6;
	int i3 = 3;
	System.p( 5  %  2 );
	System.p( 5  %  i6 );
	System.p( i6  %  5 );
	System.p( i6  %  i3 );
}
boolean int_operator_cmpEqual() { 
	int i1 = 1;
	int i2 = 2;
	System.p( 1  ==  2 );
	System.p( 1  ==  i1 );
	System.p( i1  ==  1 );
	System.p( i1  ==  i2 );
}
boolean int_operator_cmpNotEqual() { 
	int i1 = 1;
	int i2 = 2;
	System.p( 1  !=  2 );
	System.p( 1  !=  i1 );
	System.p( i1  !=  1 );
	System.p( i1  !=  i2 );
}
boolean int_operator_cmpLessThan() { 
	int i1 = 1;
	int i2 = 2;
	System.p( 1  <  2 );
	System.p( 1  <  i1 );
	System.p( i1  <  1 );
	System.p( i1  <  i2 );
}
boolean int_operator_cmpLessThanEqual() { 
	int i1 = 1;
	int i2 = 2;
	System.p( 1  <=  2 );
	System.p( 1  <=  i1 );
	System.p( i1  <=  1 );
	System.p( i1  <=  i2 );
}
boolean int_operator_cmpMoreThan() { 
	int i1 = 1;
	int i2 = 2;
	System.p( 1  >  2 );
	System.p( 1  >  i1 );
	System.p( i1  >  1 );
	System.p( i1  >  i2 );
}
boolean int_operator_cmpMoreThanEqual() { 
	int i1 = 1;
	int i2 = 2;
	System.p( 1  >=  2 );
	System.p( 1  >=  i1 );
	System.p( i1  >=  1 );
	System.p( i1  >=  i2 );
}
boolean int_operator_integerNegatives() { 
	int i0 = 0;
	int i1 = 1;
	System.p((-1) == -1);
	System.p((-(1)) == -1);
	System.p((-i0) == 0);
	System.p((-i1) == -1);
}
boolean int_operator_intOperatorPriority() { 
	int i2 = 2;
	int i3 = 3;
	int i4 = 4;
	System.p(i2 + i3 - i4);
	System.p(i2 + i3 * i4);
	System.p(i2 + i3 / i4);
	System.p(i2 + i3 % i4);
	System.p(i2 - i3 + i4);
	System.p(i2 - i3 * i4);
	System.p(i2 - i3 / i4);
	System.p(i2 - i3 % i4);
	System.p(i2 * i3 + i4);
	System.p(i2 * i3 - i4);
	System.p(i2 * i3 / i4);
	System.p(i2 * i3 % i4);
	System.p(i2 % i3 + i4);
	System.p(i2 % i3 - i4);
	System.p(i2 % i3 * i4);
	System.p(i2 % i3 / i4);
}
int_operator_add();
int_operator_sub();
int_operator_mul();
int_operator_div();
int_operator_mod();
int_operator_cmpEqual();
int_operator_cmpNotEqual();
int_operator_cmpLessThan();
int_operator_cmpLessThanEqual();
int_operator_cmpMoreThan();
int_operator_cmpMoreThanEqual();
int_operator_integerNegatives();
int_operator_intOperatorPriority();

