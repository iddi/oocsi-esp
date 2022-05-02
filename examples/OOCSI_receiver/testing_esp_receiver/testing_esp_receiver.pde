import nl.tue.id.oocsi.*;

// ******************************************************
// This example requires a running OOCSI server!
//
// How to do that? Check: Examples > Tools > LocalServer
//
// More information how to run an OOCSI server
// can be found here: https://iddi.github.io/oocsi/)
// ******************************************************

OOCSI oocsi;

void setup() {
  size(200, 200);
  background(120);
  frameRate(1);

  // connect ot OOCSI server running on the same machine (localhost)
  // with "senderName" to be my channel others can send data to
  // (for more information how to run an OOCSI server refer to: https://iddi.github.io/oocsi/)
  oocsi = new OOCSI(this, "testing-ESP-sender", "your OOCSI server address");
}

void draw() {
  // send to OOCSI ...
  oocsi
    // on channel "esp-testchannel"...
    .channel("esp-testchannel")
    // add different types of data
    .data("greeting", "hello world!")
    .data("float_point", (float) sin(frameCount/10.))
    .data("count", frameCount)
    .data("time", millis())
    .data("intArray", new int[] {1, 2, 3})
    .data("floatArray", new float[] {1.3, 4.5})
    // send finally
    .send();
}