using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CalibrationImagesCreatorByCameraRotation : MonoBehaviour
{
    [SerializeField]
    private Camera[] capturesCameras;

    public float yawMin, yawMax;
    public float pitchMin, pitchMax;
    public float maxDeltaBetweenAxis;

    public int iterationCount;

    [SerializeField]
    private string baseOutImagesPath = "Images/";
    [SerializeField]
    private string baseImagesName = "CalibrationCamera";

    private float _waitTime = 0.1f;

    private Quaternion startRotation;
    private IEnumerator SaveWaiter()
    {
        startRotation = transform.rotation;
        for (int i = 0; i < iterationCount; i++)
        {
            var pitch = Random.Range(yawMin, yawMax);
            var yaw = Random.Range(pitchMin, pitchMax);
            var rot = Quaternion.Euler(pitch, yaw, 0);
            //rot = Quaternion.Normalize(rot);
            transform.rotation = rot;
            yield return new WaitForSeconds(_waitTime);
            long unixTimestamp = (long)(System.DateTime.UtcNow.Subtract(new System.DateTime(1970, 1, 1)).TotalMilliseconds);
            int cameraIndex = 0;
            foreach(var camera in capturesCameras)
            {
                RenderTextureImageSaver.Save(camera.targetTexture, $"{baseOutImagesPath}{baseImagesName}{cameraIndex}-{unixTimestamp}");
                cameraIndex++;
            }
        }
    }
    public void StartSave()
    {
        var routine = SaveWaiter();
        StartCoroutine(routine);
    }
}
