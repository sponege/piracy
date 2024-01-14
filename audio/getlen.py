from pydub import AudioSegment

name = "noparty"

sound = AudioSegment.from_wav(f"{name}.wav")

print(len(sound))