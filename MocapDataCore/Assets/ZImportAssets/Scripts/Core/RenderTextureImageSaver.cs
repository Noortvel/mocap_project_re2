using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public static class RenderTextureImageSaver 
{
    public static void Save(RenderTexture rt, string pathName)
    {

        RenderTexture.active = rt;
        Texture2D tex = new Texture2D(rt.width, rt.height, TextureFormat.RGB24, false);
        tex.ReadPixels(new Rect(0, 0, rt.width, rt.height), 0, 0);
        RenderTexture.active = null;

        byte[] bytes;
        bytes = tex.EncodeToPNG();

        string path = $"{pathName}.png";
        System.IO.File.WriteAllBytes(path, bytes);
        AssetDatabase.ImportAsset(path);
    }
}
