<h1>Bitlow - a simple, turing-machine based programming language</h1>
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

</table>
