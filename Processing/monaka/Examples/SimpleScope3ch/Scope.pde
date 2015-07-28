/**
 * A simple oscilloscope class
 */

class Scope {
  private int l;
  private int t;
  private int h;
  private float values[];
  private int index = 0;
  private int points = 200;
  private String title;
  private int maxValue = 0;
  private int minValue = 128;

  Scope(int l, int t, int w, int h, String title) {
    this.l = l;
    this.t = t;
    this.h = h;
    this.points = w;
    this.title = title;

    values = new float[this.points];
  }
  
  public void updateAndDraw(int value) {
    values[index] = value / 128.0;
    maxValue = max(value, maxValue);
    minValue = min(value, minValue);

    smooth();

    textSize(12);
    text(title, l - 24, t - 8);
    text("127", l - 24, t + 8);
    text("  0", l - 24, t + h);
    text("val: " + value, l + points + 8, t + 8);
    text("max: " + maxValue, l + points + 8, t + 22);
    text("min: " + minValue, l + points + 8, t + 36);

    // draw outlines
    stroke(200);
    noFill();
    beginShape();
    vertex(l - 1, t - 1);
    vertex(l + points, t - 1);
    vertex(l + points, t + h);
    vertex(l - 1, t + h);
    endShape(CLOSE);
  
    // draw the signal
    stroke(255);
    beginShape();
    for (int i = 1; i < points; i++) {
      vertex(l + i, t + h - values[(index + i) % points] * (float)h);
    }
    endShape();

    index = (index + 1) % points;
  }
}
