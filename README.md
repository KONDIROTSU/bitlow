<h1>Bitlow - a simple, turing-machine based programming language</h1>
<hr>
<h3>Code written in this programming language manipulates a "tape" of bits. Operations change the currently chosen bit, referred to as "the pointer".</h3>
<h3>It uses raw binary data, in the form of hexadecimal characters, as code</h3>
<br><br>
<h2>Code manual</h2>
<table>
  <tr>
    <th>Character</th>
    <th>Definition</th>
    <th>Additional args and explanation</th>
  </tr>
  <tr>
    <th>0</th>
    <th>Changes the currently selected bit to 0</th>
    <th>-</th>
  </tr>
  <tr>
    <th>1</th>
    <th>Changes the currently selected bit to 1</th>
    <th>-</th>
  </tr>
  <tr>
    <th>2</th>
    <th>Bit selection changes to the bit on the left of the currently selected bit (pointer moves left)</th>
    <th>-</th>
  </tr>
  <tr>
    <th>3</th>
    <th>Bit selection changes to the bit on the right of the currently selected bit (pointer moves right)</th>
    <th>-</th>
  </tr>
 <tr>
    <th>4</th>
    <th>Flips the currently selected bit</th>
    <th>-</th>
  </tr>
  <tr>
    <th>5,6,7</th>
    <th>Placeholders, currently reset the tape, move the pointer to the beginning, and the end, respectively</th>
    <th>-</th>
  </tr>
  <tr>
    <th>8 and 9</th>
    <th>"{" and "}", respectively. Used to nest code inside functions.</th>
    <th>-</th>
  </tr>
  <tr>
    <th>A</th>
    <th>Tape creation</th>
    <th>The next 4 nibbles (2 bytes) is the amount of bytes in the created tape. Example: <code>A 0010</code> creates a 16-byte tape</th>
  </tr>
  <tr>
    <th>B</th>
    <th>Loop</th>
    <th>The next 4 nibbles (2 bytes) is how many times the loop of the code in brackets is done. If the value is 0, the loop is infinite. Example: <code>B 0010 8439</code> flips a bit and moves right 16 times.</th>
  </tr>
  <tr>
    <th>C</th>
    <th>"If" statement</th>
    <th>If the currently select bit is "1", the code in brackets gets executed. Example: <code>C 83129</code>; if the currently selected bit is 1, the bit to the right gets changed to 1.</th>
  </tr>
  <tr>
    <th>D</th>
    <th>Function definition</th>
    <th>Defines a function that can be called later in code. Language supports one function at one time. Function can be overwritten. Example: <code>D 8434349</code> defines a function, where the current bit and two next ones get flipped.</th>
  </tr>
  <tr>
    <th>E</th>
    <th>Executes the defined function</th>
    <th>-</th>
  </tr>
  <tr>
    <th>F</th>
    <th>Terminates the execution of the program</th>
    <th>-</th>
  </tr>
</table>
