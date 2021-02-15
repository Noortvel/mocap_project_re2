using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;


public class StereoCamerasShoter : MonoBehaviour
{
    [SerializeField]
    private Camera cameraR = null;
    [SerializeField]
    private Camera cameraL = null;
    [SerializeField]
    private string imagePrefix = null;
    [SerializeField]
    private bool isAddTimestamp = true;
    [SerializeField]
    private GameObject rootObjects = null;

    [SerializeField]
    private string baseOutImagesPath = "Assets/DepthTest1/";

    public void SaveImages()
    {
        long unixTimestamp = (long)(System.DateTime.UtcNow.Subtract(new System.DateTime(1970, 1, 1)).TotalMilliseconds);
        string timestamp = isAddTimestamp ? $"{unixTimestamp}" : "";
        RenderTextureImageSaver.Save(cameraR.targetTexture, $"{baseOutImagesPath}CameraR-{imagePrefix}-{timestamp}");
        RenderTextureImageSaver.Save(cameraL.targetTexture, $"{baseOutImagesPath}CameraL-{imagePrefix}-{timestamp}");
    }

}
