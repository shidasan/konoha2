/* this test is written by shinpei nakata */


boolean int_operator_comLessThanEqual_cmpLessThanEqual() { 
	int i1 = 1;
	int i2 = 2;
	System.p( 1  <=  2 );
	System.p( 1  <=  i1 );
	System.p( i1  <=  1 );
	System.p( i1  <=  i2 );
}
boolean int_operator_comLessThanEqual_cmpMoreThan() { 
	int i1 = 1;
	int i2 = 2;
	System.p( 1  >  2 );
	System.p( 1  >  i1 );
	System.p( i1  >  1 );
	System.p( i1  >  i2 );
}
boolean int_operator_comLessThanEqual_cmpMoreThanEqual() { 
	int i1 = 1;
	int i2 = 2;
	System.p( 1  >=  2 );
	System.p( 1  >=  i1 );
	System.p( i1  >=  1 );
	System.p( i1  >=  i2 );
}
boolean int_operator_comLessThanEqual_integerNegatives() { 
	int i0 = 0;
	int i1 = 1;
	System.p((-1) == -1);
	System.p((-(1)) == -1);
	System.p((-i0) == 0);
	System.p((-i1) == -1);
}
boolean int_operator_comLessThanEqual_intOperatorPriority() { 
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
int_operator_comLessThanEqual_cmpLessThanEqual();
int_operator_comLessThanEqual_cmpMoreThan();
int_operator_comLessThanEqual_cmpMoreThanEqual();
int_operator_comLessThanEqual_integerNegatives();
int_operator_comLessThanEqual_intOperatorPriority();

