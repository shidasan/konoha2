K.importPackage("konoha.string");

//* String.equals */
System.p("hoge" == "h");
System.p("hoge" == "hoge");
System.p("hoge" == "hogeee");
System.p("hoge" == null);
System.p("" == null);
//System.p(null == "");

//* String.startsWith */
System.p("foobar".startsWith("foo"));
System.p("foobar".startsWith("foobar"));
System.p("foobar".startsWith("foobarhoge"));
System.p("".startsWith(""));
System.p("".startsWith(null));

//* String.endsWith */
System.p("foobar".endsWith("bar"));
System.p("foobar".endsWith("foobar"));
System.p("foobar".endsWith("hogefoobar"));
System.p("".endsWith(""));
System.p("".endsWith(null));

//* String.indexOf */
System.p("foobar".indexOf("f"));
System.p("foobar".indexOf("o"));
System.p("foobar".indexOf("A"));
System.p("foobar".indexOf("bar"));
System.p("foobar".indexOf("barr"));
System.p("foobar".indexOf(null));

//* String.lastIndexOf */
System.p("foobar".lastIndexOf("f"));
System.p("foobar".lastIndexOf("o"));
System.p("foobar".lastIndexOf("A"));
System.p("foobarfoobar".lastIndexOf("bar"));
System.p("foobarfoobar".lastIndexOf("barr"));
System.p("foobar".lastIndexOf(null));

//* String.getSize */
//System.p("hoge".getSize());
//System.p("".getSize());

//* String.get */
System.p("hoge".get(2));
System.p("hoge".get(-1));
System.p("hoge".get(4));
System.p("hoge".get(-5));
System.p("".get(-1));

//* String.substring */
System.p("foobar".substring(0, 3));
System.p("foobar".substring(3, 3));
System.p("foobar".substring(3, 6));
System.p("foobar".substring(1, 0));
System.p("foobar".substring(10, 3));
System.p("foobar".substring(-2, 4));
System.p("foobar".substring(2, -2));

//* String.toUpper */
System.p("fOoBaR123".toUpper());

//* String.toLower */
System.p("fOoBaR123".toLower());

//* String.trim */
System.p("  \t\n\t foo \t\n bar \t\n\t".trim());
System.p(" \t\n\t ".trim());

//* String.concat */
System.p("foo".concat("bar"));
System.p("foo".concat(""));
System.p("foo".concat(null));
