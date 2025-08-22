<h1>Bitlow - a simple, turing-machine based programming language</h1>
<hr>
<h3>Code written in this programming language manipulates a "tape" of bits. Operations change the currently chosen bit, referred to as "the pointer".</h3>
<h3>It uses raw binary data, in the form of hexadecimal characters, as code</h3>
<br><br>
<h2>Code manual</h2>
<table>
  <tr>
    <th>Character</th>
    <th>What it does</th>
    <th>Additional args</th>
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
</table>
