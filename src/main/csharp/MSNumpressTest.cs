/*
    MSNumpressTest.cs
    rfellers@gmail.com
    Copyright 2017 Ryan Fellers

    Based on:

    MSNumpressTest.java
    johan.teleman@immun.lth.se

    Copyright 2013 Johan Teleman

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

using System;
using System.Linq;
using NUnit.Framework;

[TestFixture]
public class MSNumpressTest
{
    [Test]
    public void encodeInt()
    {
        byte[] res = new byte[10];
        int l;
        l = MSNumpress.encodeInt(0L, res, 0);
        Assert.AreEqual(1, l);
        Assert.AreEqual(8, res[0] & 0xf);

        l = MSNumpress.encodeInt(-1L, res, 0);
        Assert.AreEqual(2, l);
        Assert.AreEqual(0xf, res[0] & 0xf);
        Assert.AreEqual(0xf, res[1] & 0xf);

        l = MSNumpress.encodeInt(35L, res, 0);
        Assert.AreEqual(3, l);
        Assert.AreEqual(6, res[0] & 0xf);
        Assert.AreEqual(0x3, res[1] & 0xf);
        Assert.AreEqual(0x2, res[2] & 0xf);

        l = MSNumpress.encodeInt(370000000L, res, 0);
        Assert.AreEqual(9, l);
        Assert.AreEqual(0, res[0] & 0xf);
        Assert.AreEqual(0x0, res[1] & 0xf);
        Assert.AreEqual(0x8, res[2] & 0xf);
        Assert.AreEqual(0x0, res[3] & 0xf);
        Assert.AreEqual(0xc, res[4] & 0xf);
        Assert.AreEqual(0xd, res[5] & 0xf);
        Assert.AreEqual(0x0, res[6] & 0xf);
        Assert.AreEqual(0x6, res[7] & 0xf);
        Assert.AreEqual(0x1, res[8] & 0xf);
    }

    [Test]
    public void decodeInt()
    {
        byte[] res = new byte[10];
        res[0] = 0x75;
        res[1] = 0x87;
        res[2] = 0x10;
        res[3] = 0x08;
        res[4] = 0x0c;
        res[5] = 0xd0;
        res[6] = 0x61;
        var dec = new MSNumpress.IntDecoder(res, 0);

        long l;
        l = dec.next();
        Assert.AreEqual(5, l);

        l = dec.next();
        Assert.AreEqual(0, l);

        l = dec.next();
        Assert.AreEqual(1, l);

        l = dec.next();
        Assert.AreEqual(370000000L, l);
    }

    [Test]
    public void encodeFixedPoint()
    {
        byte[] encoded = new byte[8];
        MSNumpress.encodeFixedPoint(1.00, encoded);
        Assert.AreEqual(0x3f, 0xff & encoded[0]);
        Assert.AreEqual(0xf0, 0xff & encoded[1]);
        Assert.AreEqual(0x0, 0xff & encoded[2]);
        Assert.AreEqual(0x0, 0xff & encoded[3]);
        Assert.AreEqual(0x0, 0xff & encoded[4]);
        Assert.AreEqual(0x0, 0xff & encoded[5]);
        Assert.AreEqual(0x0, 0xff & encoded[6]);
        Assert.AreEqual(0x0, 0xff & encoded[7]);
    }

    [Test]
    public void encodeDecodeFixedPoint()
    {
        double fp = 300.21941382293625;
        byte[] encoded = new byte[8];
        MSNumpress.encodeFixedPoint(fp, encoded);
        double decoded = MSNumpress.decodeFixedPoint(encoded);
        Assert.AreEqual(fp, decoded, 0);
    }

    [Test]
    public void encodeLinear()
    {
        double[] mzs = { 100.0, 200.0, 300.00005, 400.00010 };
        byte[] encoded = new byte[40];
        int encodedBytes = MSNumpress.encodeLinear(mzs, 4, encoded, 100000.0);
        Assert.AreEqual(18, encodedBytes);
        Assert.AreEqual(0x80, 0xff & encoded[8]);
        Assert.AreEqual(0x96, 0xff & encoded[9]);
        Assert.AreEqual(0x98, 0xff & encoded[10]);
        Assert.AreEqual(0x00, 0xff & encoded[11]);
        Assert.AreEqual(0x75, 0xff & encoded[16]);
        Assert.AreEqual(0x80, 0xf0 & encoded[17]);
    }

    [Test]
    public void encodeDecodeLinearEmpty()
    {
        byte[] encoded = new byte[8];
        int encodedBytes = MSNumpress.encodeLinear(new double[0], 0, encoded, 100000.0);
        double[] decoded = new double[0];
        int decodedBytes = MSNumpress.decodeLinear(encoded, 8, decoded);
        Assert.AreEqual(0, decodedBytes);
    }

    [Test]
    public void decodeLinearNice()
    {
        double[] mzs = { 100.0, 200.0, 300.00005, 400.00010 };
        byte[] encoded = new byte[28];
        int encodedBytes = MSNumpress.encodeLinear(mzs, 4, encoded, 100000.0);
        double[] decoded = new double[4];
        int numDecoded = MSNumpress.decodeLinear(encoded, encodedBytes, decoded);
        Assert.AreEqual(4, numDecoded);
        Assert.AreEqual(100.0, decoded[0], 0.000005);
        Assert.AreEqual(200.0, decoded[1], 0.000005);
        Assert.AreEqual(300.00005, decoded[2], 0.000005);
        Assert.AreEqual(400.00010, decoded[3], 0.000005);
    }

    [Test]
    public void decodeLinearWierd()
    {
        double[] mzs = { 100.0, 200.0, 4000.00005, 0.00010 };
        byte[] encoded = new byte[28];
        double fixedPoint = MSNumpress.optimalLinearFixedPoint(mzs, 4);
        int encodedBytes = MSNumpress.encodeLinear(mzs, 4, encoded, fixedPoint);
        double[] decoded = new double[4];
        int numDecoded = MSNumpress.decodeLinear(encoded, encodedBytes, decoded);
        Assert.AreEqual(100.0, decoded[0], 0.000005);
        Assert.AreEqual(200.0, decoded[1], 0.000005);
        Assert.AreEqual(4000.00005, decoded[2], 0.000005);
        Assert.AreEqual(0.00010, decoded[3], 0.000005);
    }

    [Test]
    public void encodeDecodeLinear()
    {
        Random random = new Random();
        int n = 1000;
        double[] mzs = new double[n];
        mzs[0] = 300 + random.NextDouble();
        for (int i = 1; i < n; i++)
            mzs[i] = mzs[i - 1] + random.NextDouble();

        byte[] encoded = new byte[n * 5];
        double fixedPoint = MSNumpress.optimalLinearFixedPoint(mzs, n);
        int encodedBytes = MSNumpress.encodeLinear(mzs, n, encoded, fixedPoint);
        double[] decoded = new double[n];
        int decodedDoubles = MSNumpress.decodeLinear(encoded, encodedBytes, decoded);

        Assert.AreEqual(n, decodedDoubles);

        var list = Enumerable.Range(0, 1000).Select(i => decoded[i] / mzs[i]).ToList();

        for (int i = 0; i < n; i++)
            Assert.AreEqual(mzs[i], decoded[i], 0.000005);
    }

    [Test]
    public void encodeDecodePic()
    {
        Random random = new Random();
        int n = 1000;
        double[] ics = new double[n];
        for (int i = 0; i < n; i++)
            ics[i] = Math.Pow(10, 6 * random.NextDouble());

        byte[] encoded = new byte[n * 5];
        int encodedBytes = MSNumpress.encodePic(ics, n, encoded);
        double[] decoded = new double[n];
        int decodedDoubles = MSNumpress.decodePic(encoded, encodedBytes, decoded);

        Assert.AreEqual(n, decodedDoubles);

        for (int i = 0; i < n; i++)
            Assert.AreEqual(ics[i], decoded[i], 0.5);
    }

    [Test]
    public void encodeDecodeSlof()
    {
        Random random = new Random();
        int n = 1000;
        double[] ics = new double[n];
        for (int i = 0; i < n; i++)
            ics[i] = Math.Pow(10, 6 * random.NextDouble());

        byte[] encoded = new byte[n * 2 + 8];
        double fixedPoint = MSNumpress.optimalSlofFixedPoint(ics, n);
        int encodedBytes = MSNumpress.encodeSlof(ics, n, encoded, fixedPoint);
        double[] decoded = new double[n];
        int decodedDoubles = MSNumpress.decodeSlof(encoded, encodedBytes, decoded);

        Assert.AreEqual(n, decodedDoubles);

        for (int i = 0; i < n; i++)
            Assert.AreEqual(0.0, (ics[i] - decoded[i]) / ((ics[i] + decoded[i]) / 2), 0.0005);
    }

    [Test]
    public void encodeDecodeLinear5()
    {
        Random random = new Random();
        int n = 1000;
        double[] mzs = new double[n];
        mzs[0] = 300 + random.NextDouble();
        for (int i = 1; i < n; i++)
            mzs[i] = mzs[i - 1] + random.NextDouble();

        byte[] encoded = new byte[n * 5];
        double[] decoded = new double[n];
        double[] firstDecoded = new double[n];
        double fixedPoint = MSNumpress.optimalLinearFixedPoint(mzs, n);

        int encodedBytes = MSNumpress.encodeLinear(mzs, n, encoded, fixedPoint);
        int decodedDoubles = MSNumpress.decodeLinear(encoded, encodedBytes, decoded);

        for (int i = 0; i < n; i++)
            firstDecoded[i] = decoded[i];

        for (int i = 0; i < 5; i++)
        {
            MSNumpress.encodeLinear(decoded, n, encoded, fixedPoint);
            MSNumpress.decodeLinear(encoded, encodedBytes, decoded);
        }

        Assert.AreEqual(n, decodedDoubles);

        for (int i = 0; i < n; i++)
            Assert.AreEqual(firstDecoded[i], decoded[i], double.Epsilon);
    }

    [Test]
    public void encodeDecodePic5()
    {
        Random random = new Random();
        int n = 1000;
        double[] ics = new double[n];
        for (int i = 0; i < n; i++)
            ics[i] = Math.Pow(10, 6 * random.NextDouble());

        byte[] encoded = new byte[n * 5];
        double[] decoded = new double[n];
        double[] firstDecoded = new double[n];

        int encodedBytes = MSNumpress.encodePic(ics, n, encoded);
        int decodedDoubles = MSNumpress.decodePic(encoded, encodedBytes, decoded);

        for (int i = 0; i < n; i++)
            firstDecoded[i] = decoded[i];

        for (int i = 0; i < 5; i++)
        {
            MSNumpress.encodePic(decoded, n, encoded);
            MSNumpress.decodePic(encoded, encodedBytes, decoded);
        }

        Assert.AreEqual(n, decodedDoubles);

        for (int i = 0; i < n; i++)
            Assert.AreEqual(firstDecoded[i], decoded[i], double.Epsilon);
    }

    [Test]
    public void encodeDecodeSlof5()
    {
        Random random = new Random();
        int n = 1000;
        double[] ics = new double[n];
        for (int i = 0; i < n; i++)
            ics[i] = Math.Pow(10, 6 * random.NextDouble());

        byte[] encoded = new byte[n * 2 + 8];
        double[] decoded = new double[n];
        double[] firstDecoded = new double[n];
        double fixedPoint = MSNumpress.optimalSlofFixedPoint(ics, n);

        int encodedBytes = MSNumpress.encodeSlof(ics, n, encoded, fixedPoint);
        int decodedDoubles = MSNumpress.decodeSlof(encoded, encodedBytes, decoded);

        for (int i = 0; i < n; i++)
            firstDecoded[i] = decoded[i];

        for (int i = 0; i < 5; i++)
        {
            MSNumpress.encodeSlof(decoded, n, encoded, fixedPoint);
            MSNumpress.decodeSlof(encoded, encodedBytes, decoded);
        }

        Assert.AreEqual(n, decodedDoubles);

        for (int i = 0; i < n; i++)
            Assert.AreEqual(firstDecoded[i], decoded[i], double.Epsilon);
    }
}